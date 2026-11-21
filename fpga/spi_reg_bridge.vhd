-- ===========================================================================
-- OpenX32 - Unified SPI Control, Routing & Register Bridge
--
-- Backward-compatible with legacy OpenX32 16-bit SPI routing writes while
-- adding a full 32-bit read/write register bus (clk50_i domain) for:
--   * AES50 Port A & Port B control, status, 96 kHz mode & drop counters
--   * Consistent SPI access to BOTH Port A and Port B AES50 aux FIFOs
--   * Future OpenInferno register window (0x1000..0x1FFF)
--
-- Protocol on SPI_nCS0 / SPI_CLK / SPI_MOSI / SPI_MISO (SPI Mode 0):
--   1. Legacy 16-bit routing write (exactly 16 SCLKs while nCS low):
--        [15:8] = DATA (source channel), [7:0] = ADDR (output channel / 0xFF)
--        Commits on nCS rising edge -> pulses cfg_wr_en_o (in clk_audio_i domain).
--   2. Extended 32-bit Register Write (56 SCLKs = 7 bytes while nCS low):
--        Byte 0: 0xA5 (WRITE command)
--        Byte 1: ADDR[15:8]
--        Byte 2: ADDR[7:0]
--        Byte 3..6: WDATA[31:0] (MSB first)
--        Commits reg_we_o = '1' for 1 clk50_i cycle when bit 56 completes.
--   3. Extended 32-bit Register Read (64 SCLKs = 8 bytes while nCS low):
--        Byte 0: 0x5A (READ command)
--        Byte 1: ADDR[15:8]
--        Byte 2: ADDR[7:0]
--        Byte 3: 0x00 dummy turnaround (FPGA pulses reg_re_o, latches reg_rdata_i)
--        Byte 4..7: RDATA[31:0] shifted out MSB-first on SPI_MISO.
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_reg_bridge is
	port (
		clk50_i         : in  std_logic;
		rst_i           : in  std_logic;

		-- SPI pins (i.MX25 master, Mode 0: sample MOSI on rising SCLK, shift MISO on falling)
		spi_ncs_i       : in  std_logic;
		spi_clk_i       : in  std_logic;
		spi_mosi_i      : in  std_logic;
		spi_miso_o      : out std_logic;

		-- Legacy routing-RAM write port (synchronized to clk_audio_i = 24.576 MHz)
		clk_audio_i     : in  std_logic;
		cfg_wr_addr_o   : out std_logic_vector(7 downto 0);
		cfg_wr_data_o   : out std_logic_vector(7 downto 0);
		cfg_wr_en_o     : out std_logic;

		-- Extended 32-bit register bus (clk50_i domain)
		reg_addr_o      : out std_logic_vector(15 downto 0);
		reg_wdata_o     : out std_logic_vector(31 downto 0);
		reg_we_o        : out std_logic;
		reg_re_o        : out std_logic;
		reg_rdata_i     : in  std_logic_vector(31 downto 0)
	);
end entity spi_reg_bridge;

architecture rtl of spi_reg_bridge is

	-- 2-FF input synchronizers in clk50_i
	signal ncs_sync     : std_logic_vector(2 downto 0) := "111";
	signal sclk_sync    : std_logic_vector(2 downto 0) := "000";
	signal mosi_sync    : std_logic_vector(1 downto 0) := "00";

	signal bit_cnt      : unsigned(6 downto 0) := (others => '0');
	signal cmd_byte     : std_logic_vector(7 downto 0) := (others => '0');
	signal addr_reg     : std_logic_vector(15 downto 0) := (others => '0');
	signal data_rx      : std_logic_vector(31 downto 0) := (others => '0');
	signal legacy_rx    : std_logic_vector(15 downto 0) := (others => '0');
	signal miso_shift   : std_logic_vector(31 downto 0) := (others => '0');
	signal rd_latch_d   : std_logic_vector(1 downto 0) := "00";

	-- Toggle handshake for legacy 16-bit write -> clk_audio_i
	signal leg_tgl      : std_logic := '0';
	signal leg_addr_h   : std_logic_vector(7 downto 0) := (others => '0');
	signal leg_data_h   : std_logic_vector(7 downto 0) := (others => '0');
	signal leg_sync     : std_logic_vector(2 downto 0) := "000";

begin

	reg_addr_o  <= addr_reg;
	reg_wdata_o <= data_rx;
	spi_miso_o  <= miso_shift(31);

	process (clk50_i)
		variable sclk_rise : boolean;
		variable sclk_fall : boolean;
		variable ncs_rise  : boolean;
		variable ncs_low   : boolean;
		variable next_bit  : unsigned(6 downto 0);
	begin
		if rising_edge(clk50_i) then
			ncs_sync  <= ncs_sync(1 downto 0) & spi_ncs_i;
			sclk_sync <= sclk_sync(1 downto 0) & spi_clk_i;
			mosi_sync <= mosi_sync(0) & spi_mosi_i;

			sclk_rise := (sclk_sync(2) = '0' and sclk_sync(1) = '1');
			sclk_fall := (sclk_sync(2) = '1' and sclk_sync(1) = '0');
			ncs_rise  := (ncs_sync(2) = '0' and ncs_sync(1) = '1');
			ncs_low   := (ncs_sync(1) = '0');

			reg_we_o   <= '0';
			reg_re_o   <= '0';
			rd_latch_d <= rd_latch_d(0) & '0';

			-- Latch register read data 2 cycles after reg_re_o (supports registered slave reads)
			if rd_latch_d(1) = '1' then
				miso_shift <= reg_rdata_i;
			end if;

			if rst_i = '1' then
				bit_cnt    <= (others => '0');
				miso_shift <= (others => '0');
				leg_tgl    <= '0';
			elsif not ncs_low then
				-- On nCS rising edge: if exactly 16 bits were shifted, commit legacy routing write
				if ncs_rise and bit_cnt = 16 then
					leg_addr_h <= legacy_rx(7 downto 0);
					leg_data_h <= legacy_rx(15 downto 8);
					leg_tgl    <= not leg_tgl;
				end if;
				bit_cnt <= (others => '0');
			else
				-- Active SPI transaction (nCS = '0')
				if sclk_rise then
					next_bit := bit_cnt + 1;
					bit_cnt  <= next_bit;

					-- Always capture first 16 bits for legacy compatibility
					if bit_cnt < 16 then
						legacy_rx <= legacy_rx(14 downto 0) & mosi_sync(1);
					end if;

					-- Byte 0 (bits 0..7): command byte
					if bit_cnt < 8 then
						cmd_byte <= cmd_byte(6 downto 0) & mosi_sync(1);
					-- Bytes 1..2 (bits 8..23): 16-bit register address
					elsif bit_cnt < 24 then
						addr_reg <= addr_reg(14 downto 0) & mosi_sync(1);
						if bit_cnt = 23 and cmd_byte = x"5A" then
							-- Trigger register read on end of address phase (during dummy byte)
							reg_re_o      <= '1';
							rd_latch_d(0) <= '1';
						end if;
					-- Bytes 3..6 (bits 24..55): 32-bit write data
					elsif bit_cnt < 56 and cmd_byte = x"A5" then
						data_rx <= data_rx(30 downto 0) & mosi_sync(1);
						if bit_cnt = 55 then
							reg_we_o <= '1';
						end if;
					end if;
				end if;

				-- Shift out read data on falling SCLK during bytes 4..7 (bits 33..63)
				if sclk_fall and cmd_byte = x"5A" and bit_cnt >= 33 and bit_cnt < 64 then
					miso_shift <= miso_shift(30 downto 0) & '0';
				end if;
			end if;
		end if;
	end process;

	-- CDC toggle -> 1-clock pulse in clk_audio_i (24.576 MHz) for legacy routing RAM
	process (clk_audio_i)
	begin
		if rising_edge(clk_audio_i) then
			leg_sync    <= leg_sync(1 downto 0) & leg_tgl;
			cfg_wr_en_o <= '0';
			if leg_sync(2) /= leg_sync(1) then
				cfg_wr_addr_o <= leg_addr_h;
				cfg_wr_data_o <= leg_data_h;
				cfg_wr_en_o   <= '1';
			end if;
		end if;
	end process;

end architecture rtl;
