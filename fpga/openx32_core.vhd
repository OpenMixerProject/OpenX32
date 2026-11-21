-- ===========================================================================
-- OpenX32 - Portable Pure-VHDL Core (ECP5, Spartan-3A, and future FPGAs)
--
-- Features:
--   * 208-channel (26 x TDM8) audio routing matrix:
--       TDM  0..3  : AD0/AD1 XLR inputs  (32 ch) / DA outputs (16 ch)
--       TDM  4..7  : Expansion Card      (32 ch)
--       TDM  8     : AUX In/Out          ( 8 ch)
--       TDM  9..13 : SHARC DSP           (40 ch)
--       TDM 14..19 : AES50 Port A        (48 ch @ 48 kHz, or 24 ch @ 96 kHz + SRC)
--       TDM 20..25 : Port B              (48 ch @ 48 kHz, or 24 ch @ 96 kHz + SRC)
--   * 3 build-time Port B bitstreams via G_PORTB_MODE:
--       "AES50"   : Dual AES50v2 (48k/96k + SRC + SPI Aux on both ports)
--       "ACE"     : Allen & Heath ACE-Link on Port B
--       "INFERNO" : OpenInferno (1 ms, 48 ch @ 48 kHz) slot (Phase 2)
--   * Consistent SPI register & aux bridge for BOTH Port A and Port B
--     (100% backward-compatible with legacy 16-bit routing writes).
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity openx32_core is
	generic (
		G_PORTB_MODE    : string  := "AES50"; -- "AES50", "ACE", or "INFERNO"
		G_BOARD_TYPE    : string  := "ECP5";  -- "ECP5" or "SPARTAN3A"
		G_ENABLE_SRC_A  : boolean := true;
		G_ENABLE_SRC_B  : boolean := true;
		G_AUX_FIFO_AW   : natural := 10
	);
	port (
		-- Clocks (board wrapper provides PLL/DCM 50 MHz + external 16 MHz & 49.152 MHz)
		clk_16MHz_i             : in  std_logic;
		clk_49_152MHz_i         : in  std_logic;
		clk_50MHz_i             : in  std_logic;
		pripll_rst_o            : out std_logic;
		global_rst_o            : out std_logic;
		online_o                : out std_logic;

		-- Audio clocks out to board peripherals
		clk_24_576MHz_o         : out std_logic;
		clk_12_288MHz_o         : out std_logic;
		tdm_fs_o                : out std_logic;

		-- External CS2000-CP PLL SPI
		pll_aux_i               : in  std_logic;
		pll_in_o                : out std_logic;
		pll_ncs_o               : out std_logic;
		pll_cclk_o              : out std_logic;
		pll_cdata_o             : out std_logic;

		-- Control UART3 & SPI from i.MX25
		imx25_uart3_txd_i       : in  std_logic;
		imx25_uart3_rxd_o       : out std_logic;
		da_rx_i                 : in  std_logic;
		ad0_rx_i                : in  std_logic;
		ad1_rx_i                : in  std_logic;
		card_rx_i               : in  std_logic;
		card_rdy_i              : in  std_logic;
		da_tx_o                 : out std_logic;
		ad0_tx_o                : out std_logic;
		ad1_tx_o                : out std_logic;
		card_tx_o               : out std_logic;

		spi_ncs0_i              : in  std_logic;
		spi_clk_i               : in  std_logic;
		spi_mosi_i              : in  std_logic;
		spi_miso_o              : out std_logic;

		-- Local codec config SPI (ECP5: M8000+PCM1690; Spartan-3A: CS42438)
		aux_cclk_o              : out std_logic;
		aux_cdata_o             : out std_logic;
		aux_nrst_o              : out std_logic;
		aux_ad_ncs_o            : out std_logic; -- also AUX_nCS on Spartan-3A
		aux_da_ncs_o            : out std_logic;

		-- Local TDM8 audio I/O (AD0/1, Card, AUX, DSP)
		ad0_data0_i             : in  std_logic;
		ad0_data1_i             : in  std_logic;
		ad1_data0_i             : in  std_logic;
		ad1_data1_i             : in  std_logic;
		card_in_i               : in  std_logic_vector(3 downto 0);
		aux_ad_i                : in  std_logic;
		dsp_dout_i              : in  std_logic_vector(4 downto 0); -- 0..3 + aux

		da_data0_o              : out std_logic;
		da_data1_o              : out std_logic;
		card_out_o              : out std_logic_vector(3 downto 0);
		aux_da_o                : out std_logic;
		dsp_din_o               : out std_logic_vector(4 downto 0); -- 0..3 + aux

		-- Ultranet P16 A & B
		p16_a_txp_o             : out std_logic;
		p16_a_txm_o             : out std_logic;
		p16_b_txp_o             : out std_logic;
		p16_b_txm_o             : out std_logic;

		-- AES50 Port A (IOB-registered RMII from board wrapper + LVDS clocks)
		aes50a_rmii_crs_dv_i    : in  std_logic;
		aes50a_rmii_rxd_i       : in  std_logic_vector(1 downto 0);
		aes50a_rmii_tx_en_o     : out std_logic;
		aes50a_rmii_txd_o       : out std_logic_vector(1 downto 0);
		aes50a_phy_rst_n_o      : out std_logic;
		aes50a_clk_a_rx_i       : in  std_logic;
		aes50a_clk_a_tx_o       : out std_logic;
		aes50a_clk_a_tx_en_o    : out std_logic;
		aes50a_clk_a_rx_nen_o   : out std_logic;
		aes50a_clk_b_rx_i       : in  std_logic;
		aes50a_clk_b_tx_o       : out std_logic;
		aes50a_clk_b_tx_en_o    : out std_logic;
		aes50a_clk_b_rx_nen_o   : out std_logic;

		-- Port B (IOB-registered RMII from board wrapper + LVDS clocks)
		aes50b_rmii_crs_dv_i    : in  std_logic;
		aes50b_rmii_rxd_i       : in  std_logic_vector(1 downto 0);
		aes50b_rmii_er_i        : in  std_logic;
		aes50b_rmii_tx_en_o     : out std_logic;
		aes50b_rmii_txd_o       : out std_logic_vector(1 downto 0);
		aes50b_clk_a_rx_i       : in  std_logic;
		aes50b_clk_a_tx_o       : out std_logic;
		aes50b_clk_a_tx_en_o    : out std_logic;
		aes50b_clk_a_rx_nen_o   : out std_logic;
		aes50b_clk_b_rx_i       : in  std_logic;
		aes50b_clk_b_tx_o       : out std_logic;
		aes50b_clk_b_tx_en_o    : out std_logic;
		aes50b_clk_b_rx_nen_o   : out std_logic;

		-- External register bus for Phase 2 OpenInferno (0x1000..0x1FFF, clk_50MHz_i)
		ext_reg_addr_o          : out std_logic_vector(11 downto 0);
		ext_reg_wdata_o         : out std_logic_vector(31 downto 0);
		ext_reg_we_o            : out std_logic;
		ext_reg_re_o            : out std_logic;
		ext_reg_rdata_i         : in  std_logic_vector(31 downto 0) := (others => '0')
	);
end entity openx32_core;

architecture rtl of openx32_core is

	constant C_NUM_TDM   : integer := 26;  -- 26 x TDM8 = 208 channels
	constant C_NUM_CH    : integer := 208; -- 32 AD + 32 Card + 8 AUX + 40 DSP + 48 AES50A + 48 PortB

	-- Resets & audio clocks
	signal pripll_rst    : std_logic;
	signal rst           : std_logic;
	signal online        : std_logic;
	signal aes50_base_rst: std_logic;
	signal clk_24_576MHz : std_logic;
	signal clk_12_288MHz : std_logic;
	signal tdm_fs        : std_logic;

	-- Matrix TDM buses (26 x TDM8 = 208 ch)
	signal tdm_input     : std_logic_vector(C_NUM_TDM - 1 downto 0) := (others => '0');
	signal tdm_output    : std_logic_vector(C_NUM_TDM - 1 downto 0);
	signal matrix_in     : std_logic_vector(C_NUM_TDM * 24 - 1 downto 0);
	signal matrix_out    : std_logic_vector(C_NUM_TDM * 24 - 1 downto 0);
	signal ram_wr_en     : std_logic;
	signal ram_wr_addr   : std_logic_vector(7 downto 0);
	signal ram_wr_data   : std_logic_vector(23 downto 0);
	signal ram_rd_addr   : std_logic_vector(7 downto 0);
	signal ram_rd_data   : std_logic_vector(23 downto 0);
	signal ch_idx_out    : std_logic_vector(7 downto 0);

	-- SPI bridge & legacy routing
	signal cfg_wr_en     : std_logic;
	signal cfg_wr_addr   : std_logic_vector(7 downto 0);
	signal cfg_wr_data   : std_logic_vector(7 downto 0);
	signal reg_addr      : std_logic_vector(15 downto 0);
	signal reg_wdata     : std_logic_vector(31 downto 0);
	signal reg_we        : std_logic;
	signal reg_re        : std_logic;
	signal reg_rdata     : std_logic_vector(31 downto 0) := (others => '0');
	signal inf_reg_rdata : std_logic_vector(31 downto 0) := (others => '0');

	component inferno_port_x32
		generic (
			MAX_CH  : integer := 48;
			NTXF    : integer := 16;
			NRXF    : integer := 16;
			MAXFCH  : integer := 32;
			MAX_FPP : integer := 16;
			NR      : integer := 64;
			NF      : integer := 64;
			MAXLAT  : integer := 1;
			TXW     : integer := 512;
			RXW     : integer := 2048
		);
		port (
			clk50_i       : in  std_logic;
			rst_i         : in  std_logic;
			bclk_i        : in  std_logic;
			fsync_i       : in  std_logic;
			rmii_crs_dv_i : in  std_logic;
			rmii_rxd_i    : in  std_logic_vector(1 downto 0);
			rmii_tx_en_o  : out std_logic;
			rmii_txd_o    : out std_logic_vector(1 downto 0);
			tdm_i         : in  std_logic_vector(5 downto 0);
			tdm_o         : out std_logic_vector(5 downto 0);
			reg_addr_i    : in  std_logic_vector(11 downto 0);
			reg_wdata_i   : in  std_logic_vector(31 downto 0);
			reg_we_i      : in  std_logic;
			reg_re_i      : in  std_logic;
			reg_rdata_o   : out std_logic_vector(31 downto 0)
		);
	end component;

	-- Control & status registers (clk_50MHz_i domain)
	signal porta_fs96      : std_logic := '0';
	signal portb_fs96      : std_logic := '0';
	signal porta_sys_mode  : std_logic_vector(1 downto 0) := "01"; -- AES50 Master
	signal portb_sys_mode  : std_logic_vector(1 downto 0) := "01"; -- AES50 Master
	signal porta_soft_rst  : std_logic := '0';
	signal portb_soft_rst  : std_logic := '0';

	signal porta_aes_ok, porta_rx_ok, porta_tx_st, porta_drop, porta_fmt_v : std_logic := '0';
	signal portb_aes_ok, portb_rx_ok, portb_tx_st, portb_drop, portb_fmt_v : std_logic := '0';
	signal porta_fmt, portb_fmt : std_logic_vector(7 downto 0) := (others => '0');
	signal porta_drop_cnt, portb_drop_cnt : unsigned(31 downto 0) := (others => '0');

	-- SPI Aux FIFO signals for Port A and Port B
	signal porta_aux_tx_we, porta_aux_rx_rd, porta_aux_rx_v : std_logic := '0';
	signal portb_aux_tx_we, portb_aux_rx_rd, portb_aux_rx_v : std_logic := '0';
	signal porta_aux_rx_b, portb_aux_rx_b : std_logic_vector(7 downto 0) := (others => '0');
	signal porta_aux_tx_cnt, porta_aux_rx_cnt : std_logic_vector(15 downto 0) := (others => '0');
	signal portb_aux_tx_cnt, portb_aux_rx_cnt : std_logic_vector(15 downto 0) := (others => '0');

	-- Clock source mux & CS2000-CP command engine (0x0040..0x0048, matches inferno-portd CS2K_*)
	signal clk_src          : std_logic_vector(1 downto 0) := "00"; -- 00=16MHz, 01=AES50A, 10=AES50B
	signal porta_clk_to_pll : std_logic := '0';
	signal portb_clk_to_pll : std_logic := '0';
	signal pll_lock_s       : std_logic_vector(1 downto 0) := "00";
	signal cs2k_boot_done   : std_logic := '0';
	signal cs2k_boot_done_s : std_logic_vector(1 downto 0) := "00";
	signal cs2k_cmd         : std_logic_vector(15 downto 0) := (others => '0');
	signal cs2k_busy        : std_logic := '0';
	signal cs2k_req_tgl     : std_logic := '0';
	signal cs2k_req_s       : std_logic_vector(2 downto 0) := "000";
	signal cs2k_ack_tgl     : std_logic := '0';
	signal cs2k_ack_s       : std_logic_vector(2 downto 0) := "000";
	type cs2k_host_st_t is (CS2K_IDLE, CS2K_WAIT_START, CS2K_WAIT_DONE);
	signal cs2k_host_st     : cs2k_host_st_t := CS2K_IDLE;
	signal host_pll_start   : std_logic := '0';

	-- Codec SPI config wires
	signal boot_pll_start, pll_spi_start, pll_spi_busy : std_logic;
	signal boot_pll_addr, boot_pll_data, boot_pll_map : std_logic_vector(7 downto 0);
	signal pll_spi_addr, pll_spi_data, pll_spi_map : std_logic_vector(7 downto 0);
	signal adc_start, adc_busy, adc_done, adc_cclk, adc_cdata : std_logic;
	signal adc_addr, adc_data, adc_map : std_logic_vector(7 downto 0);
	signal dac_start, dac_busy, dac_cclk, dac_cdata : std_logic;
	signal dac_addr, dac_data : std_logic_vector(7 downto 0);

	-- Ultranet P16 demuxed channels
	signal p16a_ch1, p16a_ch2, p16a_ch3, p16a_ch4, p16a_ch5, p16a_ch6, p16a_ch7, p16a_ch8 : std_logic_vector(23 downto 0);
	signal p16b_ch1, p16b_ch2, p16b_ch3, p16b_ch4, p16b_ch5, p16b_ch6, p16b_ch7, p16b_ch8 : std_logic_vector(23 downto 0);

	function portb_mode_id return std_logic_vector is
	begin
		if G_PORTB_MODE = "AES50" then return x"1";
		elsif G_PORTB_MODE = "INFERNO" then return x"2";
		elsif G_PORTB_MODE = "ACE" then return x"3";
		else return x"0";
		end if;
	end function;

begin

	-- ================================================================
	-- Clocks, Resets & Simple Pass-throughs
	-- ================================================================
	pripll_rst_o    <= pripll_rst;
	global_rst_o    <= rst;
	online_o        <= online;
	clk_24_576MHz_o <= clk_24_576MHz;
	clk_12_288MHz_o <= clk_12_288MHz;
	tdm_fs_o        <= tdm_fs;
	pll_in_o        <= porta_clk_to_pll when clk_src = "01" else
	                   portb_clk_to_pll when clk_src = "10" else
	                   clk_16MHz_i;

	da_tx_o         <= imx25_uart3_txd_i;
	ad0_tx_o        <= imx25_uart3_txd_i;
	ad1_tx_o        <= imx25_uart3_txd_i;
	card_tx_o       <= imx25_uart3_txd_i;

	u_reset : entity work.reset
		port map (
			clk          => clk_16MHz_i,
			o_pripll_rst => pripll_rst,
			o_secpll_rst => open,
			o_global_rst => rst,
			o_online     => online
		);

	u_aes50_rst : entity work.aes50_rst
		port map (
			clk50_i => clk_50MHz_i,
			start_i => online,
			rst_o   => aes50_base_rst
		);

	u_audioclk : entity work.audioclk
		port map (
			fs_x_1024_i => clk_49_152MHz_i,
			fs_x_512_o  => clk_24_576MHz,
			fs_x_256_o  => clk_12_288MHz,
			fs_x_128_o  => open,
			fs_x_64_o   => open,
			fs_o        => tdm_fs
		);

	u_uart_col : entity work.uart_collector
		port map (
			clk_in   => clk_16MHz_i,
			rst_in   => rst,
			uart1_in => da_rx_i,
			uart2_in => ad0_rx_i,
			uart3_in => ad1_rx_i,
			uart4_in => card_rx_i,
			card_rdy => card_rdy_i,
			uart_out => imx25_uart3_rxd_o
		);

	-- ================================================================
	-- External CS2000-CP PLL & Local AUX Codec Configuration
	-- ================================================================
	u_cs2000 : entity work.cs2000cp_config
		port map (
			clk       => clk_16MHz_i,
			i_start   => pripll_rst,
			i_txbusy  => pll_spi_busy,
			o_start   => boot_pll_start,
			o_address => boot_pll_addr,
			o_data    => boot_pll_data,
			o_map     => boot_pll_map,
			o_done    => cs2k_boot_done
		);

	-- Host CS2000-CP SPI transfer FSM (clk_16MHz_i domain, active after boot init completes)
	process (clk_16MHz_i)
	begin
		if rising_edge(clk_16MHz_i) then
			host_pll_start <= '0';
			cs2k_req_s     <= cs2k_req_s(1 downto 0) & cs2k_req_tgl;
			case cs2k_host_st is
				when CS2K_IDLE =>
					if cs2k_boot_done = '1' and cs2k_req_s(2) /= cs2k_req_s(1) then
						host_pll_start <= '1';
						cs2k_host_st   <= CS2K_WAIT_START;
					end if;
				when CS2K_WAIT_START =>
					if pll_spi_busy = '1' then
						cs2k_host_st <= CS2K_WAIT_DONE;
					end if;
				when CS2K_WAIT_DONE =>
					if pll_spi_busy = '0' then
						cs2k_ack_tgl <= not cs2k_ack_tgl;
						cs2k_host_st <= CS2K_IDLE;
					end if;
			end case;
		end if;
	end process;

	pll_spi_start <= host_pll_start when cs2k_boot_done = '1' else boot_pll_start;
	pll_spi_addr  <= x"9E"                when cs2k_boot_done = '1' else boot_pll_addr;
	pll_spi_map   <= cs2k_cmd(15 downto 8) when cs2k_boot_done = '1' else boot_pll_map;
	pll_spi_data  <= cs2k_cmd(7 downto 0)  when cs2k_boot_done = '1' else boot_pll_data;

	u_pll_spi : entity work.spi_tx
		port map (
			clk       => clk_16MHz_i,
			i_start   => pll_spi_start,
			i_address => pll_spi_addr,
			i_data    => pll_spi_data,
			i_map     => pll_spi_map,
			o_nCS     => pll_ncs_o,
			o_cclk    => pll_cclk_o,
			o_cdata   => pll_cdata_o,
			o_busy    => pll_spi_busy
		);

	gen_ecp5_codecs : if G_BOARD_TYPE = "ECP5" generate
		aux_nrst_o <= online;
		u_m8000 : entity work.m8000adc_config
			port map (
				clk => clk_16MHz_i, i_start => online, i_txbusy => adc_busy,
				o_start => adc_start, o_done => adc_done,
				o_address => adc_addr, o_data => adc_data, o_map => adc_map
			);
		u_m8000_spi : entity work.spi_tx
			port map (
				clk => clk_16MHz_i, i_start => adc_start,
				i_address => adc_addr, i_data => adc_data, i_map => adc_map,
				o_nCS => aux_ad_ncs_o, o_cclk => adc_cclk, o_cdata => adc_cdata, o_busy => adc_busy
			);
		u_pcm1690 : entity work.pcm1690dac_config
			port map (
				clk => clk_16MHz_i, i_start => adc_done, i_txbusy => dac_busy,
				o_start => dac_start, o_done => open, o_address => dac_addr, o_data => dac_data
			);
		u_pcm1690_spi : entity work.spi_16bit_tx
			port map (
				clk => clk_16MHz_i, i_start => dac_start, i_address => dac_addr, i_data => dac_data,
				o_nCS => aux_da_ncs_o, o_cclk => dac_cclk, o_cdata => dac_cdata, o_busy => dac_busy
			);
		aux_cclk_o  <= adc_cclk or dac_cclk;
		aux_cdata_o <= adc_cdata or dac_cdata;
	end generate;

	gen_s3a_codecs : if G_BOARD_TYPE = "SPARTAN3A" generate
		aux_da_ncs_o <= '1';
		u_cs42438 : entity work.cs42438_config
			port map (
				clk => clk_16MHz_i, i_start => online, i_txbusy => adc_busy,
				o_start => adc_start, o_nRST => aux_nrst_o,
				o_address => adc_addr, o_data => adc_data, o_map => adc_map
			);
		u_cs42438_spi : entity work.spi_tx
			port map (
				clk => clk_16MHz_i, i_start => adc_start,
				i_address => adc_addr, i_data => adc_data, i_map => adc_map,
				o_nCS => aux_ad_ncs_o, o_cclk => aux_cclk_o, o_cdata => aux_cdata_o, o_busy => adc_busy
			);
	end generate;

	-- ================================================================
	-- Unified SPI Register & Routing Bridge (Port A + Port B + Inferno)
	-- ================================================================
	u_spi_bridge : entity work.spi_reg_bridge
		port map (
			clk50_i       => clk_50MHz_i,
			rst_i         => aes50_base_rst,
			spi_ncs_i     => spi_ncs0_i,
			spi_clk_i     => spi_clk_i,
			spi_mosi_i    => spi_mosi_i,
			spi_miso_o    => spi_miso_o,
			clk_audio_i   => clk_24_576MHz,
			cfg_wr_addr_o => cfg_wr_addr,
			cfg_wr_data_o => cfg_wr_data,
			cfg_wr_en_o   => cfg_wr_en,
			reg_addr_o    => reg_addr,
			reg_wdata_o   => reg_wdata,
			reg_we_o      => reg_we,
			reg_re_o      => reg_re,
			reg_rdata_i   => reg_rdata
		);

	ext_reg_addr_o  <= reg_addr(11 downto 0);
	ext_reg_wdata_o <= reg_wdata;
	ext_reg_we_o    <= reg_we when reg_addr(15 downto 12) = x"1" else '0';
	ext_reg_re_o    <= reg_re when reg_addr(15 downto 12) = x"1" else '0';

	-- System & AES50 Register Bank (0x0000..0x00FF, clk_50MHz_i)
	process (clk_50MHz_i)
		variable src_flags : std_logic_vector(1 downto 0);
	begin
		if rising_edge(clk_50MHz_i) then
			porta_aux_tx_we <= '0';
			porta_aux_rx_rd <= '0';
			portb_aux_tx_we <= '0';
			portb_aux_rx_rd <= '0';

			if aes50_base_rst = '1' then
				porta_fs96     <= '0';
				portb_fs96     <= '0';
				porta_sys_mode <= "01";
				portb_sys_mode <= "01";
				porta_soft_rst <= '0';
				portb_soft_rst <= '0';
				porta_drop_cnt <= (others => '0');
				portb_drop_cnt <= (others => '0');
				clk_src        <= "00";
				cs2k_cmd       <= (others => '0');
				cs2k_busy      <= '0';
			else
				pll_lock_s       <= pll_lock_s(0) & pll_aux_i;
				cs2k_boot_done_s <= cs2k_boot_done_s(0) & cs2k_boot_done;
				cs2k_ack_s       <= cs2k_ack_s(1 downto 0) & cs2k_ack_tgl;
				if cs2k_ack_s(2) /= cs2k_ack_s(1) then
					cs2k_busy <= '0';
				end if;

				if porta_drop = '1' then porta_drop_cnt <= porta_drop_cnt + 1; end if;
				if portb_drop = '1' then portb_drop_cnt <= portb_drop_cnt + 1; end if;

				-- Register writes
				if reg_we = '1' and reg_addr(15 downto 8) = x"00" then
					case reg_addr(7 downto 0) is
						when x"08" =>
							porta_fs96     <= reg_wdata(0);
							portb_fs96     <= reg_wdata(1);
							porta_sys_mode <= reg_wdata(3 downto 2);
							portb_sys_mode <= reg_wdata(5 downto 4);
							porta_soft_rst <= reg_wdata(8);
							portb_soft_rst <= reg_wdata(9);
							clk_src        <= reg_wdata(11 downto 10);
						when x"20" =>
							porta_aux_tx_we <= '1';
						when x"30" =>
							portb_aux_tx_we <= '1';
						when x"40" =>
							cs2k_cmd <= reg_wdata(15 downto 0);
						when x"48" =>
							if reg_wdata(0) = '1' and cs2k_busy = '0' and cs2k_boot_done_s(1) = '1' then
								cs2k_req_tgl <= not cs2k_req_tgl;
								cs2k_busy    <= '1';
							end if;
						when others =>
							null;
					end case;
				end if;

				-- Register reads
				if reg_re = '1' then
					if reg_addr(15 downto 12) = x"1" then
						if G_PORTB_MODE = "INFERNO" then
							reg_rdata <= inf_reg_rdata;
						else
							reg_rdata <= ext_reg_rdata_i;
						end if;
					elsif reg_addr(15 downto 8) = x"00" then
						src_flags := "00";
						if G_ENABLE_SRC_A then src_flags(0) := '1'; end if;
						if G_ENABLE_SRC_B then src_flags(1) := '1'; end if;
						case reg_addr(7 downto 0) is
							when x"00" => reg_rdata <= x"4F583332"; -- "OX32"
							when x"04" => reg_rdata <= x"0000" & portb_mode_id & "10" & src_flags & x"02"; -- bit 11 = CS2K engine
							when x"08" => reg_rdata <= x"00000" & clk_src & portb_soft_rst & porta_soft_rst &
							                           "00" & portb_sys_mode & porta_sys_mode & portb_fs96 & porta_fs96;
							when x"0C" => reg_rdata <= portb_fmt & "0000" & portb_fmt_v & portb_tx_st & portb_rx_ok & portb_aes_ok &
							                           porta_fmt & pll_lock_s(1) & "000" & porta_fmt_v & porta_tx_st & porta_rx_ok & porta_aes_ok;
							when x"10" => reg_rdata <= std_logic_vector(porta_drop_cnt);
							when x"14" => reg_rdata <= std_logic_vector(portb_drop_cnt);
							when x"24" =>
								reg_rdata <= porta_aux_rx_cnt & "0000000" & porta_aux_rx_v & porta_aux_rx_b;
								if porta_aux_rx_v = '1' then porta_aux_rx_rd <= '1'; end if;
							when x"28" => reg_rdata <= porta_aux_tx_cnt & porta_aux_rx_cnt;
							when x"34" =>
								reg_rdata <= portb_aux_rx_cnt & "0000000" & portb_aux_rx_v & portb_aux_rx_b;
								if portb_aux_rx_v = '1' then portb_aux_rx_rd <= '1'; end if;
							when x"38" => reg_rdata <= portb_aux_tx_cnt & portb_aux_rx_cnt;
							when x"40" => reg_rdata <= x"0000" & cs2k_cmd;
							when x"44" => reg_rdata <= (cs2k_busy or not cs2k_boot_done_s(1)) & "000000000000000" & cs2k_cmd;
							when others => reg_rdata <= (others => '0');
						end case;
					else
						reg_rdata <= (others => '0');
					end if;
				end if;
			end if;
		end if;
	end process;

	-- ================================================================
	-- 208-Channel (26 x TDM8) Audio Matrix
	-- ================================================================
	tdm_input(0)  <= ad0_data1_i;
	tdm_input(1)  <= ad0_data0_i;
	tdm_input(2)  <= ad1_data1_i;
	tdm_input(3)  <= ad1_data0_i;
	tdm_input(4)  <= card_in_i(3);
	tdm_input(5)  <= card_in_i(2);
	tdm_input(6)  <= card_in_i(1);
	tdm_input(7)  <= card_in_i(0);
	tdm_input(8)  <= aux_ad_i;
	tdm_input(13 downto 9) <= dsp_dout_i;

	da_data1_o <= tdm_output(0);
	da_data0_o <= tdm_output(1);
	card_out_o(0) <= tdm_output(4);
	card_out_o(1) <= tdm_output(5);
	card_out_o(2) <= tdm_output(6);
	card_out_o(3) <= tdm_output(7);
	aux_da_o      <= tdm_output(8);
	dsp_din_o     <= tdm_output(13 downto 9);

	u_tdm_rx : entity work.tdm_multichan_rx
		generic map (TDM_RECEIVER => C_NUM_TDM, DATA_WIDTH => 24)
		port map (bclk => clk_12_288MHz, fsync => tdm_fs, tdm_in => tdm_input, data_out => matrix_in);

	u_tdm_tx : entity work.tdm_multichan_tx
		generic map (TDM_RECEIVER => C_NUM_TDM, DATA_WIDTH => 24)
		port map (bclk => clk_12_288MHz, fsync => tdm_fs, data_in => matrix_out, tdm_out => tdm_output);

	u_am_wr : entity work.audiomatrix_ram_write
		generic map (TDM_RECEIVER => C_NUM_TDM, DATA_WIDTH => 24, NUM_INPUT_PORTS => C_NUM_CH, ADDR_WIDTH => 8)
		port map (
			clk => clk_24_576MHz, bclk => clk_12_288MHz, fsync => tdm_fs, input_data => matrix_in,
			o_ram_write_addr => ram_wr_addr, o_ram_data => ram_wr_data, o_ram_wr_en => ram_wr_en
		);

	u_am_ram : entity work.audiomatrix_ram
		generic map (DATA_WIDTH => 24, ADDR_WIDTH => 8, RAM_DEPTH => 256)
		port map (
			clk => clk_24_576MHz, wr_en => ram_wr_en,
			write_addr => ram_wr_addr, i_data => ram_wr_data,
			read_addr => ram_rd_addr, o_data => ram_rd_data
		);

	u_am_rram : entity work.audiomatrix_routing_ram
		generic map (ADDR_WIDTH => 8, RAM_DEPTH => 256)
		port map (
			clk => clk_24_576MHz, cfg_wr_en => cfg_wr_en,
			cfg_wr_addr => cfg_wr_addr, cfg_wr_data => cfg_wr_data,
			channel_idx => ch_idx_out, read_addr => ram_rd_addr
		);

	u_am_rd : entity work.audiomatrix_ram_read
		generic map (TDM_SENDER => C_NUM_TDM, DATA_WIDTH => 24, NUM_OUTPUT_PORTS => C_NUM_CH, ADDR_WIDTH => 8)
		port map (
			clk => clk_24_576MHz, bclk => clk_12_288MHz, fsync => tdm_fs, ram_data => ram_rd_data,
			channel_idx_out => ch_idx_out, data_out => matrix_out
		);

	-- Ultranet P16 A (TDM 2) & B (TDM 3)
	u_p16a_demux : entity work.tdm_demux
		port map (
			bclk => clk_12_288MHz, fsync => tdm_fs, data_in => matrix_out(71 downto 48),
			ch1_out => p16a_ch1, ch2_out => p16a_ch2, ch3_out => p16a_ch3, ch4_out => p16a_ch4,
			ch5_out => p16a_ch5, ch6_out => p16a_ch6, ch7_out => p16a_ch7, ch8_out => p16a_ch8
		);
	u_p16b_demux : entity work.tdm_demux
		port map (
			bclk => clk_12_288MHz, fsync => tdm_fs, data_in => matrix_out(95 downto 72),
			ch1_out => p16b_ch1, ch2_out => p16b_ch2, ch3_out => p16b_ch3, ch4_out => p16b_ch4,
			ch5_out => p16b_ch5, ch6_out => p16b_ch6, ch7_out => p16b_ch7, ch8_out => p16b_ch8
		);
	u_p16a : entity work.ultranet_tx
		port map (
			bit_clock => clk_24_576MHz,
			ch1 => p16a_ch1, ch2 => p16a_ch2, ch3 => p16a_ch3, ch4 => p16a_ch4,
			ch5 => p16a_ch5, ch6 => p16a_ch6, ch7 => p16a_ch7, ch8 => p16a_ch8,
			ultranet_out_p => p16_a_txp_o, ultranet_out_m => p16_a_txm_o
		);
	u_p16b : entity work.ultranet_tx
		port map (
			bit_clock => clk_24_576MHz,
			ch1 => p16b_ch1, ch2 => p16b_ch2, ch3 => p16b_ch3, ch4 => p16b_ch4,
			ch5 => p16b_ch5, ch6 => p16b_ch6, ch7 => p16b_ch7, ch8 => p16b_ch8,
			ultranet_out_p => p16_b_txp_o, ultranet_out_m => p16_b_txm_o
		);

	-- ================================================================
	-- AES50 Port A (Always AES50v2: 48 kHz 48ch / 96 kHz 24ch + SRC + SPI Aux)
	-- ================================================================
	aes50a_clk_a_rx_nen_o <= '0';
	aes50a_clk_b_rx_nen_o <= '0';

	u_aes50a : entity work.aes50_par_bridge
		generic map (
			G_ENABLE_SRC  => G_ENABLE_SRC_A,
			G_ENABLE_AUX  => true,
			G_AUX_FIFO_AW => G_AUX_FIFO_AW
		)
		port map (
			clk50_i             => clk_50MHz_i,
			rst_i               => aes50_base_rst or porta_soft_rst,
			clk_1024xfs_i       => clk_49_152MHz_i,
			bclk_i              => clk_12_288MHz,
			fsync_i             => tdm_fs,
			fs96_i              => porta_fs96,
			sys_mode_i          => porta_sys_mode,
			rmii_crs_dv_i       => aes50a_rmii_crs_dv_i,
			rmii_rxd_i          => aes50a_rmii_rxd_i,
			rmii_tx_en_o        => aes50a_rmii_tx_en_o,
			rmii_txd_o          => aes50a_rmii_txd_o,
			phy_rst_n_o         => aes50a_phy_rst_n_o,
			aes50_clk_a_rx_i    => aes50a_clk_a_rx_i,
			aes50_clk_a_tx_o    => aes50a_clk_a_tx_o,
			aes50_clk_a_tx_en_o => aes50a_clk_a_tx_en_o,
			aes50_clk_b_rx_i    => aes50a_clk_b_rx_i,
			aes50_clk_b_tx_o    => aes50a_clk_b_tx_o,
			aes50_clk_b_tx_en_o => aes50a_clk_b_tx_en_o,
			pll_lock_n_i        => not pll_aux_i,
			clk_to_pll_o        => porta_clk_to_pll,
			tdm_i               => tdm_output(19 downto 14),
			tdm_o               => tdm_input(19 downto 14),
			aux_tx_we_i         => porta_aux_tx_we,
			aux_tx_byte_i       => reg_wdata(7 downto 0),
			aux_tx_count_o      => porta_aux_tx_cnt,
			aux_rx_rd_i         => porta_aux_rx_rd,
			aux_rx_byte_o       => porta_aux_rx_b,
			aux_rx_valid_o      => porta_aux_rx_v,
			aux_rx_count_o      => porta_aux_rx_cnt,
			aes_ok_o            => porta_aes_ok,
			rx_ok_o             => porta_rx_ok,
			tx_started_o        => porta_tx_st,
			rx_block_drop_o     => porta_drop,
			fmt_valid_o         => porta_fmt_v,
			fmt_o               => porta_fmt
		);

	-- ================================================================
	-- Port B: Build-Time Selectable ("AES50" / "ACE" / "INFERNO")
	-- ================================================================
	gen_portb_aes50 : if G_PORTB_MODE = "AES50" generate
		aes50b_clk_a_rx_nen_o <= '0';
		aes50b_clk_b_rx_nen_o <= '0';

		u_aes50b : entity work.aes50_par_bridge
			generic map (
				G_ENABLE_SRC  => G_ENABLE_SRC_B,
				G_ENABLE_AUX  => true,
				G_AUX_FIFO_AW => G_AUX_FIFO_AW
			)
			port map (
				clk50_i             => clk_50MHz_i,
				rst_i               => aes50_base_rst or portb_soft_rst,
				clk_1024xfs_i       => clk_49_152MHz_i,
				bclk_i              => clk_12_288MHz,
				fsync_i             => tdm_fs,
				fs96_i              => portb_fs96,
				sys_mode_i          => portb_sys_mode,
				rmii_crs_dv_i       => aes50b_rmii_crs_dv_i,
				rmii_rxd_i          => aes50b_rmii_rxd_i,
				rmii_tx_en_o        => aes50b_rmii_tx_en_o,
				rmii_txd_o          => aes50b_rmii_txd_o,
				phy_rst_n_o         => open,
				aes50_clk_a_rx_i    => aes50b_clk_a_rx_i,
				aes50_clk_a_tx_o    => aes50b_clk_a_tx_o,
				aes50_clk_a_tx_en_o => aes50b_clk_a_tx_en_o,
				aes50_clk_b_rx_i    => aes50b_clk_b_rx_i,
				aes50_clk_b_tx_o    => aes50b_clk_b_tx_o,
				aes50_clk_b_tx_en_o => aes50b_clk_b_tx_en_o,
				pll_lock_n_i        => not pll_aux_i,
				clk_to_pll_o        => portb_clk_to_pll,
				tdm_i               => tdm_output(25 downto 20),
				tdm_o               => tdm_input(25 downto 20),
				aux_tx_we_i         => portb_aux_tx_we,
				aux_tx_byte_i       => reg_wdata(7 downto 0),
				aux_tx_count_o      => portb_aux_tx_cnt,
				aux_rx_rd_i         => portb_aux_rx_rd,
				aux_rx_byte_o       => portb_aux_rx_b,
				aux_rx_valid_o      => portb_aux_rx_v,
				aux_rx_count_o      => portb_aux_rx_cnt,
				aes_ok_o            => portb_aes_ok,
				rx_ok_o             => portb_rx_ok,
				tx_started_o        => portb_tx_st,
				rx_block_drop_o     => portb_drop,
				fmt_valid_o         => portb_fmt_v,
				fmt_o               => portb_fmt
			);
	end generate;

	gen_portb_ace : if G_PORTB_MODE = "ACE" generate
		signal eth_clk_1kHz   : std_logic;
		signal mac_rst        : std_logic;
		signal mac_tx_clk     : std_logic;
		signal mac_tx_busy    : std_logic;
		signal mac_tx_sent    : std_logic;
		signal ace_tx_en      : std_logic;
		signal ace_tx_busy    : std_logic;
		signal ace_tx_data    : std_logic_vector(7 downto 0);
		signal ace_ready      : std_logic;
		signal ace_audio      : std_logic_vector(1559 downto 0);
		signal src_mac        : std_logic_vector(47 downto 0);
		signal src_mac_rev    : std_logic_vector(47 downto 0);
	begin
		-- Disable AES50B LVDS transceivers in Ethernet/ACE mode
		aes50b_clk_a_tx_o     <= '0';
		aes50b_clk_b_tx_o     <= '0';
		aes50b_clk_a_tx_en_o  <= '0';
		aes50b_clk_b_tx_en_o  <= '0';
		aes50b_clk_a_rx_nen_o <= '1';
		aes50b_clk_b_rx_nen_o <= '1';
		tdm_input(25 downto 20) <= (others => '0');

		u_clk1k : entity work.clk_by_x
			generic map (divider => 16000)
			port map (clk_in => clk_16MHz_i, clk_out => eth_clk_1kHz);

		u_eth_rst : entity work.ethernet_reset
			port map (clk => eth_clk_1kHz, power_good => online, mac_rst => mac_rst);

		u_eth_cfg : entity work.const_eth_config
			port map (src_mac_address => src_mac);

		u_rev_mac : entity work.reverse_mac
			port map (mac_address_i => src_mac, mac_address_o => src_mac_rev);

		u_ace_demux : entity work.tdm_ace_demux
			port map (
				bclk => clk_12_288MHz, fsync => tdm_fs, ace_tx_busy => ace_tx_busy,
				tdm0_in => matrix_out(503 downto 480),
				tdm1_in => matrix_out(527 downto 504),
				tdm2_in => matrix_out(551 downto 528),
				tdm3_in => matrix_out(575 downto 552),
				tdm4_in => matrix_out(599 downto 576),
				tdm5_in => matrix_out(623 downto 600),
				tdm6_in => (others => '0'),
				tdm7_in => (others => '0'),
				audio_ready => ace_ready, audio_out => ace_audio
			);

		u_ace_pkt : entity work.ace_audio_packet
			port map (
				tx_clk => mac_tx_clk, tx_busy => mac_tx_busy, tx_byte_sent => mac_tx_sent,
				audio_ready => ace_ready, audio_in => ace_audio, src_mac_address => src_mac,
				tx_enable => ace_tx_en, ace_tx_busy => ace_tx_busy, tx_data => ace_tx_data
			);

		u_eth_mac : entity work.ethernet_rmii
			generic map (MIIM_DISABLE => false, MIIM_CLOCK_DIVIDER => 50, MIIM_POLL_WAIT_TICKS => 10000000, MIIM_RESET_WAIT_TICKS => 0)
			port map (
				reset_i => mac_rst, rmii_clk_i => clk_50MHz_i,
				rmii_rx_er_i => aes50b_rmii_er_i, rmii_rx_crs_dv_i => aes50b_rmii_crs_dv_i,
				rmii_rxd_i => aes50b_rmii_rxd_i, miim_clock_i => clk_50MHz_i,
				mac_address_i => src_mac_rev, tx_enable_i => ace_tx_en, tx_data_i => ace_tx_data,
				rmii_tx_en_o => aes50b_rmii_tx_en_o, rmii_txd_o => aes50b_rmii_txd_o,
				tx_clock_o => mac_tx_clk, tx_byte_sent_o => mac_tx_sent, tx_busy_o => mac_tx_busy
			);
	end generate;

	gen_portb_inferno : if G_PORTB_MODE = "INFERNO" generate
		-- Spartan-3A (32 RAMB16) uses 4 KB CPU RX queue (RXW=1024, -2 RAMB16) while
		-- keeping required 1 ms max latency (MAXLAT=1, NF=64, NR=64, MAX_CH=48).
		function inferno_rxw return integer is
		begin
			if G_BOARD_TYPE = "SPARTAN3A" then
				return 1024;
			else
				return 2048;
			end if;
		end function;
		signal inf_we : std_logic;
		signal inf_re : std_logic;
	begin
		-- Disable AES50B LVDS transceivers in Ethernet/Inferno mode
		aes50b_clk_a_tx_o     <= '0';
		aes50b_clk_b_tx_o     <= '0';
		aes50b_clk_a_tx_en_o  <= '0';
		aes50b_clk_b_tx_en_o  <= '0';
		aes50b_clk_a_rx_nen_o <= '1';
		aes50b_clk_b_rx_nen_o <= '1';

		inf_we <= reg_we when reg_addr(15 downto 12) = x"1" else '0';
		inf_re <= reg_re when reg_addr(15 downto 12) = x"1" else '0';

		u_inferno_b : inferno_port_x32
			generic map (
				MAX_CH  => 48,
				MAX_FPP => 16,
				NR      => 64,
				NF      => 64,
				MAXLAT  => 1,
				TXW     => 512,
				RXW     => inferno_rxw
			)
			port map (
				clk50_i       => clk_50MHz_i,
				rst_i         => aes50_base_rst or portb_soft_rst,
				bclk_i        => clk_12_288MHz,
				fsync_i       => tdm_fs,
				rmii_crs_dv_i => aes50b_rmii_crs_dv_i,
				rmii_rxd_i    => aes50b_rmii_rxd_i,
				rmii_tx_en_o  => aes50b_rmii_tx_en_o,
				rmii_txd_o    => aes50b_rmii_txd_o,
				tdm_i         => tdm_output(25 downto 20),
				tdm_o         => tdm_input(25 downto 20),
				reg_addr_i    => reg_addr(11 downto 0),
				reg_wdata_i   => reg_wdata,
				reg_we_i      => inf_we,
				reg_re_i      => inf_re,
				reg_rdata_o   => inf_reg_rdata
			);
	end generate;

end architecture rtl;
