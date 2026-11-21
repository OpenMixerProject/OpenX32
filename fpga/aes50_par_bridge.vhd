-- ===========================================================================
-- OpenX32 - AES50v2 Parallel Bridge (48 kHz 48ch + 96 kHz 24ch SRC + SPI Aux)
--
-- Wraps unmodified upstream YetAnotherElectronicsChannel/AES50v2:
--   * aes50_par_core     (48 kHz 48ch & 96 kHz 24ch AES50 engine)
--   * aes50_aux_uart_enc / aes50_aux_uart_dec / aes50_uart (Behringer/Midas aux)
--   * src_96k_48k        (47-tap synchronous half-band FIR SRC for 96k stageboxes)
--
-- Interfaces:
--   * Audio: 6x TDM8 (48 ch) with 1-BCLK fsync delay (OpenX32 matrix format)
--   * Aux:   SPI byte FIFO interface in clk50_i domain (consistent for Port A & B)
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_par_bridge is
	generic (
		G_ENABLE_SRC        : boolean := true;  -- include 96<->48 kHz 24ch half-band SRC
		G_ENABLE_AUX        : boolean := true;  -- include SPI-accessible Behringer/Midas aux tunnel
		G_AUX_FIFO_AW       : natural := 10;    -- 1023-byte SPI aux TX/RX FIFOs
		G_SIM_ASSM_INIT     : natural := 2;     -- pass-through to aes50_par_core (simulation speedup)
		G_SIM_ASSM_FRAMES   : natural := 2048;
		G_SIM_WD_CLK        : natural := 25;
		G_SIM_WD_RX_DV      : natural := 7500;
		G_SIM_MDIX_1MS      : natural := 50000;
		G_SIM_CLK_OK_REF    : natural := 500000
	);
	port (
		clk50_i             : in  std_logic;
		rst_i               : in  std_logic;
		clk_1024xfs_i       : in  std_logic; -- 49.152 MHz audio PLL
		bclk_i              : in  std_logic; -- 12.288 MHz TDM bit clock
		fsync_i             : in  std_logic; -- 48 kHz TDM frame sync

		-- Runtime config (change while rst_i = '1')
		fs96_i              : in  std_logic; -- '0' = 48 kHz (48 ch), '1' = 96 kHz (24 ch + SRC)
		sys_mode_i          : in  std_logic_vector(1 downto 0); -- "01" Master, "00" Slave

		-- RMII PHY
		rmii_crs_dv_i       : in  std_logic;
		rmii_rxd_i          : in  std_logic_vector(1 downto 0);
		rmii_tx_en_o        : out std_logic;
		rmii_txd_o          : out std_logic_vector(1 downto 0);
		phy_rst_n_o         : out std_logic;

		-- AES50 LVDS clock transceivers
		aes50_clk_a_rx_i    : in  std_logic;
		aes50_clk_a_tx_o    : out std_logic;
		aes50_clk_a_tx_en_o : out std_logic;
		aes50_clk_b_rx_i    : in  std_logic;
		aes50_clk_b_tx_o    : out std_logic;
		aes50_clk_b_tx_en_o : out std_logic;
		pll_lock_n_i        : in  std_logic;
		clk_to_pll_o        : out std_logic;

		-- 6x TDM8 audio interface to OpenX32 matrix (1-BCLK delay after rising fsync)
		tdm_i               : in  std_logic_vector(5 downto 0);
		tdm_o               : out std_logic_vector(5 downto 0);

		-- SPI Aux byte FIFO interface (clk50_i domain)
		aux_tx_we_i         : in  std_logic;
		aux_tx_byte_i       : in  std_logic_vector(7 downto 0);
		aux_tx_count_o      : out std_logic_vector(15 downto 0);
		aux_rx_rd_i         : in  std_logic;
		aux_rx_byte_o       : out std_logic_vector(7 downto 0);
		aux_rx_valid_o      : out std_logic;
		aux_rx_count_o      : out std_logic_vector(15 downto 0);

		-- Status & diagnostics (clk50_i domain)
		aes_ok_o            : out std_logic;
		rx_ok_o             : out std_logic;
		tx_started_o        : out std_logic;
		rx_block_drop_o     : out std_logic;
		fmt_valid_o         : out std_logic;
		fmt_o               : out std_logic_vector(7 downto 0)
	);
end entity aes50_par_bridge;

architecture rtl of aes50_par_bridge is

	-- Internal fast UART bit time for SPI-to-aux-encoder/decoder (50 MHz / 16 = 3.125 Mbps)
	constant C_INT_UART_CLKS    : natural := 16;
	constant C_INT_UART_TIMEOUT : natural := 4096; -- ~82 us idle flush

	-- ------------------------------------------------------------
	-- TDM8 <-> Dual-clock 64x24 RAMs (BCLK <-> clk50)
	-- ------------------------------------------------------------
	type line6_t is array (0 to 5) of std_logic_vector(23 downto 0);
	signal wclk_prev      : std_logic := '0';
	signal bit_pos        : unsigned(7 downto 0) := (others => '0');
	signal tdm_in_sh      : line6_t := (others => (others => '0'));
	signal tdm_out_sh     : line6_t := (others => (others => '0'));
	signal tdm_out_bit    : std_logic_vector(5 downto 0) := (others => '0');

	signal tdm_tx_we      : std_logic := '0';
	signal tdm_tx_waddr   : unsigned(5 downto 0) := (others => '0');
	signal tdm_tx_wdata   : std_logic_vector(23 downto 0) := (others => '0');
	signal tdm_tx_raddr   : unsigned(5 downto 0) := (others => '0');
	signal tdm_tx_rdata   : std_logic_vector(23 downto 0);

	signal tdm_rx_we      : std_logic := '0';
	signal tdm_rx_waddr   : unsigned(5 downto 0) := (others => '0');
	signal tdm_rx_wdata   : std_logic_vector(23 downto 0) := (others => '0');
	signal tdm_rx_raddr   : unsigned(5 downto 0) := (others => '0');
	signal tdm_rx_rdata   : std_logic_vector(23 downto 0);

	-- ------------------------------------------------------------
	-- aes50_par_core signals
	-- ------------------------------------------------------------
	signal sample_tick    : std_logic;
	signal par_in_we      : std_logic := '0';
	signal par_in_ch      : std_logic_vector(5 downto 0) := (others => '0');
	signal par_in_sample  : std_logic_vector(23 downto 0) := (others => '0');
	signal par_out_valid  : std_logic;
	signal par_out_ch     : std_logic_vector(5 downto 0);
	signal par_out_phase  : std_logic;
	signal par_out_sample : std_logic_vector(23 downto 0);

	signal aux_tx_req     : std_logic;
	signal aux_tx_we_c    : std_logic := '0';
	signal aux_tx_data_c  : std_logic_vector(15 downto 0) := (others => '0');
	signal aux_rx_cnt_c   : unsigned(5 downto 0);
	signal aux_rx_rd_c    : std_logic := '0';
	signal aux_rx_first_c : std_logic;
	signal aux_rx_data_c  : std_logic_vector(15 downto 0);

	signal rx_block_p     : std_logic;
	signal fs96_eff       : std_logic;

	-- ------------------------------------------------------------
	-- TX streaming & 96k ping-pong holding RAMs (2 banks x 32 x 24)
	-- ------------------------------------------------------------
	type ram64_t is array (0 to 63) of std_logic_vector(23 downto 0);
	signal tx_p0_ram      : ram64_t := (others => (others => '0'));
	signal tx_p1_ram      : ram64_t := (others => (others => '0'));
	signal tx_p0_rdata    : std_logic_vector(23 downto 0) := (others => '0');
	signal tx_p1_rdata    : std_logic_vector(23 downto 0) := (others => '0');
	signal tx_p0_raddr    : unsigned(5 downto 0) := (others => '0');
	signal tx_p1_raddr    : unsigned(5 downto 0) := (others => '0');
	signal tx_wr_bank     : std_logic := '0';
	signal tx_rd_bank     : std_logic := '1';

	type tx_st_t is (TX_IDLE, TX_RD48, TX_SEND48, TX_RD96, TX_SEND96_P1);
	signal tx_st          : tx_st_t := TX_IDLE;
	signal tx_ch_cnt      : unsigned(5 downto 0) := (others => '0');
	signal tx_96_half     : std_logic := '0';
	signal tx_rd_d        : std_logic := '0';

	-- ------------------------------------------------------------
	-- RX playout ring (8 periods x 64 words = 512 x 24)
	-- ------------------------------------------------------------
	signal play_we        : std_logic := '0';
	signal play_waddr     : unsigned(8 downto 0) := (others => '0');
	signal play_wdata     : std_logic_vector(23 downto 0) := (others => '0');
	signal play_raddr     : unsigned(8 downto 0) := (others => '0');
	signal play_rdata     : std_logic_vector(23 downto 0);

	signal rx_wr_period   : unsigned(2 downto 0) := (others => '0');
	signal rx_rd_period   : unsigned(2 downto 0) := (others => '0');
	signal rx_wr_slot_cnt : unsigned(5 downto 0) := (others => '0');
	signal rx_ring_level  : unsigned(3 downto 0) := (others => '0');

	type rx_st_t is (RX_IDLE, RX_PLAY48, RX_FEED96_P0, RX_FEED96_P1);
	signal rx_st          : rx_st_t := RX_IDLE;
	signal rx_ch_cnt      : unsigned(5 downto 0) := (others => '0');
	signal rx_rd_d        : std_logic := '0';
	signal rx_96_half     : std_logic := '0';

	-- ------------------------------------------------------------
	-- SRC signals
	-- ------------------------------------------------------------
	signal src_rx_we      : std_logic := '0';
	signal src_rx_phase   : std_logic := '0';
	signal src_rx_ch      : unsigned(4 downto 0) := (others => '0');
	signal src_rx_sample  : std_logic_vector(23 downto 0) := (others => '0');
	signal src_rx_run     : std_logic := '0';
	signal src_rx_oval    : std_logic := '0';
	signal src_rx_och     : unsigned(4 downto 0) := (others => '0');
	signal src_rx_osamp   : std_logic_vector(23 downto 0) := (others => '0');

	signal src_tx_we      : std_logic := '0';
	signal src_tx_ch      : unsigned(4 downto 0) := (others => '0');
	signal src_tx_sample  : std_logic_vector(23 downto 0) := (others => '0');
	signal src_tx_run     : std_logic := '0';
	signal src_tx_oval    : std_logic := '0';
	signal src_tx_ophase  : std_logic := '0';
	signal src_tx_och     : unsigned(4 downto 0) := (others => '0');
	signal src_tx_osamp   : std_logic_vector(23 downto 0) := (others => '0');

	-- ------------------------------------------------------------
	-- SPI Aux <-> upstream aes50_aux_uart_enc/dec signals
	-- ------------------------------------------------------------
	signal spi_tx_empty   : std_logic := '1';
	signal spi_tx_rd      : std_logic := '0';
	signal spi_tx_data    : std_logic_vector(7 downto 0) := (others => '0');
	signal spi_tx_cnt     : unsigned(3 downto 0) := (others => '0');
	signal int_utx_start  : std_logic := '0';
	signal int_utx_busy   : std_logic := '0';
	signal int_utx_line   : std_logic := '1';
	signal int_utx_fetch  : unsigned(1 downto 0) := "00";

	signal enc_aux_we     : std_logic := '0';
	signal enc_aux_addr   : unsigned(7 downto 0) := (others => '0');
	signal enc_aux_data   : std_logic_vector(15 downto 0) := (others => '0');
	signal enc_done       : std_logic := '0';
	signal enc_req        : std_logic := '0';
	signal enc_page       : std_logic := '0';
	signal aux_page_we    : std_logic := '0';
	signal aux_page_raddr : unsigned(7 downto 0) := (others => '0');
	signal aux_page_rdata : std_logic_vector(15 downto 0) := (others => '0');
	signal aux_drain_on   : std_logic := '0';
	signal aux_drain_idx  : unsigned(5 downto 0) := (others => '0');
	signal aux_drain_d    : std_logic := '0';
	signal aux_send_page  : std_logic := '0';

	signal dec_aux_we     : std_logic := '0';
	signal dec_aux_start  : std_logic := '0';
	signal dec_aux_data   : std_logic_vector(15 downto 0) := (others => '0');
	signal dec_uart_line  : std_logic := '1';
	signal int_urx_valid  : std_logic := '0';
	signal int_urx_byte   : std_logic_vector(7 downto 0) := (others => '0');
	signal spi_rx_empty   : std_logic := '1';
	signal spi_rx_ne_d    : std_logic := '0';
	signal spi_rx_cnt     : unsigned(G_AUX_FIFO_AW - 1 downto 0) := (others => '0');
	signal aux_rx_pipe    : std_logic_vector(1 downto 0) := "00";
	signal aux_rx_pace    : unsigned(4 downto 0) := (others => '0');

begin

	fs96_eff <= fs96_i when G_ENABLE_SRC else '0';

	-- ================================================================
	-- 1. Upstream AES50v2 Parallel Core (48 kHz 48ch / 96 kHz 24ch)
	-- ================================================================
	u_par_core : entity work.aes50_par_core
		generic map (
			G_ASSM_INIT   => G_SIM_ASSM_INIT,
			G_ASSM_FRAMES => G_SIM_ASSM_FRAMES,
			G_WD_CLK      => G_SIM_WD_CLK,
			G_WD_RX_DV    => G_SIM_WD_RX_DV,
			G_MDIX_1MS    => G_SIM_MDIX_1MS,
			G_CLK_OK_REF  => G_SIM_CLK_OK_REF
		)
		port map (
			clk50_i                 => clk50_i,
			rst_i                   => rst_i,
			fs96_i                  => fs96_eff,
			sys_mode_i              => sys_mode_i,
			rmii_crs_dv_i           => rmii_crs_dv_i,
			rmii_rxd_i              => rmii_rxd_i,
			rmii_tx_en_o            => rmii_tx_en_o,
			rmii_txd_o              => rmii_txd_o,
			phy_rst_n_o             => phy_rst_n_o,
			aes50_clk_a_rx_i        => aes50_clk_a_rx_i,
			aes50_clk_a_tx_o        => aes50_clk_a_tx_o,
			aes50_clk_a_tx_en_o     => aes50_clk_a_tx_en_o,
			aes50_clk_b_rx_i        => aes50_clk_b_rx_i,
			aes50_clk_b_tx_o        => aes50_clk_b_tx_o,
			aes50_clk_b_tx_en_o     => aes50_clk_b_tx_en_o,
			clk_1024xfs_from_pll_i  => clk_1024xfs_i,
			pll_lock_n_i            => pll_lock_n_i,
			clk_to_pll_o            => clk_to_pll_o,
			sample_tick_o           => sample_tick,
			par_in_we_i             => par_in_we,
			par_in_ch_i             => par_in_ch,
			par_in_sample_i         => par_in_sample,
			par_out_valid_o         => par_out_valid,
			par_out_ch_o            => par_out_ch,
			par_out_phase_o         => par_out_phase,
			par_out_sample_o        => par_out_sample,
			aux_tx_req_o            => aux_tx_req,
			aux_tx_we_i             => aux_tx_we_c,
			aux_tx_data_i           => aux_tx_data_c,
			aux_rx_count_o          => aux_rx_cnt_c,
			aux_rx_rd_i             => aux_rx_rd_c,
			aux_rx_first_o          => aux_rx_first_c,
			aux_rx_data_o           => aux_rx_data_c,
			aes_ok_o                => aes_ok_o,
			rx_ok_o                 => rx_ok_o,
			tx_started_o            => tx_started_o,
			tx_frame_o              => open,
			rx_frame_ok_o           => open,
			rx_frame_err_o          => open,
			rx_block_o              => rx_block_p,
			rx_block_drop_o         => rx_block_drop_o,
			rx_assm_o               => open,
			fmt_valid_o             => fmt_valid_o,
			fmt_bad_o               => open,
			fmt_o                   => fmt_o
		);

	-- ================================================================
	-- 2. Optional 47-tap Synchronous 96 <-> 48 kHz Half-Band FIR SRC
	-- ================================================================
	gen_src : if G_ENABLE_SRC generate
		u_src : entity work.src_96k_48k
			generic map (G_NUM_CH => 24)
			port map (
				clk_i           => clk50_i,
				rst_i           => rst_i,
				rx_we_i         => src_rx_we,
				rx_phase_i      => src_rx_phase,
				rx_ch_i         => src_rx_ch,
				rx_sample_i     => src_rx_sample,
				rx_run_i        => src_rx_run,
				rx_out_valid_o  => src_rx_oval,
				rx_out_ch_o     => src_rx_och,
				rx_out_sample_o => src_rx_osamp,
				tx_we_i         => src_tx_we,
				tx_ch_i         => src_tx_ch,
				tx_sample_i     => src_tx_sample,
				tx_run_i        => src_tx_run,
				tx_out_valid_o  => src_tx_oval,
				tx_out_phase_o  => src_tx_ophase,
				tx_out_ch_o     => src_tx_och,
				tx_out_sample_o => src_tx_osamp
			);
	end generate;

	-- ================================================================
	-- 3. Dual-Clock TDM8 <-> Parallel Sample RAMs (64 x 24 bit each)
	-- ================================================================
	u_tdm_tx_ram : entity work.aes50_dpram
		generic map (G_AW => 6, G_DW => 24)
		port map (
			wclk_i => bclk_i, we_i => tdm_tx_we, waddr_i => tdm_tx_waddr, wdata_i => tdm_tx_wdata,
			rclk_i => clk50_i, raddr_i => tdm_tx_raddr, rdata_o => tdm_tx_rdata
		);

	u_tdm_rx_ram : entity work.aes50_dpram
		generic map (G_AW => 6, G_DW => 24)
		port map (
			wclk_i => clk50_i, we_i => tdm_rx_we, waddr_i => tdm_rx_waddr, wdata_i => tdm_rx_wdata,
			rclk_i => bclk_i, raddr_i => tdm_rx_raddr, rdata_o => tdm_rx_rdata
		);

	-- 6-period RX playout ring (8 x 64 = 512 x 24 bit, single clock clk50_i)
	u_play_ram : entity work.aes50_dpram
		generic map (G_AW => 9, G_DW => 24)
		port map (
			wclk_i => clk50_i, we_i => play_we, waddr_i => play_waddr, wdata_i => play_wdata,
			rclk_i => clk50_i, raddr_i => play_raddr, rdata_o => play_rdata
		);

	-- ================================================================
	-- 4. BCLK-Domain 6x TDM8 SerDes (OpenX32 1-BCLK FSYNC delay)
	-- ================================================================
	process (bclk_i)
	begin
		if falling_edge(bclk_i) then
			tdm_o <= tdm_out_bit;
		end if;
	end process;

	process (bclk_i)
		variable cap_pos   : unsigned(7 downto 0);
		variable tx_pos    : unsigned(7 downto 0);
		variable slot_bit  : unsigned(4 downto 0);
		variable tx_bit    : unsigned(4 downto 0);
		variable slot      : unsigned(2 downto 0);
		variable next_slot : unsigned(2 downto 0);
		variable line      : natural range 0 to 5;
	begin
		if rising_edge(bclk_i) then
			wclk_prev <= fsync_i;
			tdm_tx_we <= '0';

			-- 1-BCLK delay after rising fsync_i (matches OpenX32 tdm_multichan_tx/rx)
			if fsync_i = '1' and wclk_prev = '0' then
				cap_pos := to_unsigned(255, 8);
			else
				cap_pos := bit_pos;
			end if;
			tx_pos   := cap_pos + 1;
			bit_pos  <= tx_pos;
			slot_bit := cap_pos(4 downto 0);
			tx_bit   := tx_pos(4 downto 0);
			slot     := cap_pos(7 downto 5);

			-- Shift in 6 TDM lines (bits 0..23)
			if slot_bit <= 23 then
				for i in 0 to 5 loop
					tdm_in_sh(i) <= tdm_in_sh(i)(22 downto 0) & tdm_i(i);
				end loop;
			end if;

			-- Write 6 captured samples to tdm_tx_ram at slot_bit 24..29
			if slot_bit >= 24 and slot_bit <= 29 then
				line         := to_integer(slot_bit) - 24;
				tdm_tx_we    <= '1';
				tdm_tx_waddr <= to_unsigned(line * 8, 6) + resize(slot, 6);
				tdm_tx_wdata <= tdm_in_sh(line);
			end if;

			-- Read next slot's 6 samples from tdm_rx_ram at slot_bit 22..27, load at 24..29
			next_slot := slot + 1;
			if slot_bit >= 22 and slot_bit <= 27 then
				line         := to_integer(slot_bit) - 22;
				tdm_rx_raddr <= to_unsigned(line * 8, 6) + resize(next_slot, 6);
			end if;
			if slot_bit >= 24 and slot_bit <= 29 then
				line := to_integer(slot_bit) - 24;
				tdm_out_sh(line) <= tdm_rx_rdata;
			end if;

			-- Shift out MSB first 1 BCLK ahead (falling_edge registers tdm_o for next rising_edge)
			if tx_bit <= 23 then
				for i in 0 to 5 loop
					tdm_out_bit(i) <= tdm_out_sh(i)(23);
					tdm_out_sh(i)  <= tdm_out_sh(i)(22 downto 0) & '0';
				end loop;
			else
				tdm_out_bit <= (others => '0');
			end if;
		end if;
	end process;

	-- ================================================================
	-- 5. TX Streaming FSM (tdm_tx_ram -> [SRC] -> aes50_par_core)
	-- ================================================================
	process (clk50_i)
	begin
		if rising_edge(clk50_i) then
			par_in_we   <= '0';
			src_tx_we   <= '0';
			src_tx_run  <= '0';
			tx_rd_d     <= '0';
			tx_p0_rdata <= tx_p0_ram(to_integer(tx_p0_raddr));
			tx_p1_rdata <= tx_p1_ram(to_integer(tx_p1_raddr));

			if rst_i = '1' then
				tx_st      <= TX_IDLE;
				tx_96_half <= '0';
				tx_wr_bank <= '0';
				tx_rd_bank <= '1';
			else
				-- Capture SRC TX interpolator outputs into current write bank (624 clk < 1041 clk 48k period)
				if G_ENABLE_SRC and src_tx_oval = '1' then
					if src_tx_ophase = '0' then
						tx_p0_ram(to_integer(tx_wr_bank & src_tx_och)) <= src_tx_osamp;
					else
						tx_p1_ram(to_integer(tx_wr_bank & src_tx_och)) <= src_tx_osamp;
					end if;
				end if;

				case tx_st is
					when TX_IDLE =>
						if sample_tick = '1' then
							if fs96_eff = '0' then
								tx_ch_cnt    <= (others => '0');
								tdm_tx_raddr <= (others => '0');
								tx_st        <= TX_RD48;
							elsif tx_96_half = '0' then
								tx_96_half   <= '1';
								tx_rd_bank   <= tx_wr_bank;
								tx_wr_bank   <= not tx_wr_bank;
								tx_ch_cnt    <= (others => '0');
								tdm_tx_raddr <= (others => '0');
								tx_p0_raddr  <= tx_wr_bank & to_unsigned(0, 5);
								tx_rd_d      <= '0';
								tx_st        <= TX_RD96;
							else
								tx_96_half  <= '0';
								tx_ch_cnt   <= (others => '0');
								tx_p1_raddr <= tx_rd_bank & to_unsigned(0, 5);
								tx_rd_d     <= '0';
								tx_st       <= TX_SEND96_P1;
							end if;
						end if;

					when TX_RD48 =>
						tdm_tx_raddr <= tx_ch_cnt + 1;
						tx_st        <= TX_SEND48;

					when TX_SEND48 =>
						par_in_we     <= '1';
						par_in_ch     <= std_logic_vector(tx_ch_cnt);
						par_in_sample <= tdm_tx_rdata;
						if tx_ch_cnt = 47 then
							tx_st <= TX_IDLE;
						else
							tx_ch_cnt    <= tx_ch_cnt + 1;
							tdm_tx_raddr <= tx_ch_cnt + 2;
						end if;

					when TX_RD96 =>
						-- Stream 24 new channels into SRC TX interpolator while playing
						-- previous period's phase-0 bank into aes50_par_core
						if tx_rd_d = '1' then
							src_tx_we     <= '1';
							src_tx_ch     <= tx_ch_cnt(4 downto 0);
							src_tx_sample <= tdm_tx_rdata;
							par_in_we     <= '1';
							par_in_ch     <= std_logic_vector(tx_ch_cnt);
							par_in_sample <= tx_p0_rdata;
							if tx_ch_cnt = 23 then
								src_tx_run <= '1';
								tx_st      <= TX_IDLE;
							else
								tx_ch_cnt    <= tx_ch_cnt + 1;
								tdm_tx_raddr <= tx_ch_cnt + 2;
								tx_p0_raddr  <= tx_rd_bank & (tx_ch_cnt(4 downto 0) + 2);
								tx_rd_d      <= '1';
							end if;
						else
							tdm_tx_raddr <= to_unsigned(1, 6);
							tx_p0_raddr  <= tx_rd_bank & to_unsigned(1, 5);
							tx_rd_d      <= '1';
						end if;

					when TX_SEND96_P1 =>
						if tx_rd_d = '1' then
							par_in_we     <= '1';
							par_in_ch     <= std_logic_vector(tx_ch_cnt);
							par_in_sample <= tx_p1_rdata;
							if tx_ch_cnt = 23 then
								tx_st <= TX_IDLE;
							else
								tx_ch_cnt   <= tx_ch_cnt + 1;
								tx_p1_raddr <= tx_rd_bank & (tx_ch_cnt(4 downto 0) + 2);
								tx_rd_d     <= '1';
							end if;
						else
							tx_p1_raddr <= tx_rd_bank & to_unsigned(1, 5);
							tx_rd_d     <= '1';
						end if;
				end case;
			end if;
		end if;
	end process;

	-- ================================================================
	-- 6. RX Playout Ring & Decimator FSM (aes50_par_core -> [SRC] -> tdm_rx_ram)
	-- ================================================================
	process (clk50_i)
		variable slot6      : unsigned(5 downto 0);
		variable wr_inc     : boolean;
		variable rd_inc     : boolean;
		variable level_next : unsigned(3 downto 0);
	begin
		if rising_edge(clk50_i) then
			play_we    <= '0';
			tdm_rx_we  <= '0';
			src_rx_we  <= '0';
			src_rx_run <= '0';
			wr_inc     := false;
			rd_inc     := false;

			if rst_i = '1' then
				rx_wr_period   <= (others => '0');
				rx_rd_period   <= (others => '0');
				rx_wr_slot_cnt <= (others => '0');
				rx_ring_level  <= (others => '0');
				rx_st          <= RX_IDLE;
				rx_96_half     <= '0';
			else
				-- Write incoming burst from aes50_par_core (48 words per 48k period)
				if par_out_valid = '1' then
					if fs96_eff = '1' and par_out_phase = '1' then
						slot6 := unsigned(par_out_ch) + to_unsigned(24, 6);
					else
						slot6 := unsigned(par_out_ch);
					end if;
					play_we    <= '1';
					play_waddr <= rx_wr_period & slot6;
					play_wdata <= par_out_sample;
					if rx_wr_slot_cnt = 47 then
						rx_wr_slot_cnt <= (others => '0');
						rx_wr_period   <= rx_wr_period + 1;
						wr_inc         := true;
					else
						rx_wr_slot_cnt <= rx_wr_slot_cnt + 1;
					end if;
				end if;

				-- Write decimated SRC outputs into tdm_rx_ram (ch 0..23)
				if G_ENABLE_SRC and src_rx_oval = '1' then
					tdm_rx_we    <= '1';
					tdm_rx_waddr <= '0' & src_rx_och;
					tdm_rx_wdata <= src_rx_osamp;
				end if;

				-- Play 1 period per 48 kHz tick
				case rx_st is
					when RX_IDLE =>
						if sample_tick = '1' then
							if fs96_eff = '1' then
								rx_96_half <= not rx_96_half;
							end if;
							if (fs96_eff = '0' or rx_96_half = '0') and rx_ring_level > 0 then
								rx_ch_cnt  <= (others => '0');
								play_raddr <= rx_rd_period & to_unsigned(0, 6);
								rx_rd_d    <= '0';
								if fs96_eff = '0' then
									rx_st <= RX_PLAY48;
								else
									rx_st <= RX_FEED96_P0;
								end if;
							end if;
						end if;

					when RX_PLAY48 =>
						if rx_rd_d = '1' then
							tdm_rx_we    <= '1';
							tdm_rx_waddr <= rx_ch_cnt;
							tdm_rx_wdata <= play_rdata;
							if rx_ch_cnt = 47 then
								rx_rd_period <= rx_rd_period + 1;
								rd_inc       := true;
								rx_st        <= RX_IDLE;
							else
								rx_ch_cnt  <= rx_ch_cnt + 1;
								play_raddr <= rx_rd_period & (rx_ch_cnt + 2);
							end if;
						else
							play_raddr <= rx_rd_period & to_unsigned(1, 6);
							rx_rd_d    <= '1';
						end if;

					when RX_FEED96_P0 =>
						-- Feed phase 0 (slots 0..23) into SRC decimator
						if rx_rd_d = '1' then
							src_rx_we     <= '1';
							src_rx_phase  <= '0';
							src_rx_ch     <= rx_ch_cnt(4 downto 0);
							src_rx_sample <= play_rdata;
							if rx_ch_cnt = 23 then
								rx_ch_cnt  <= (others => '0');
								rx_rd_d    <= '0';
								play_raddr <= rx_rd_period & to_unsigned(24, 6);
								rx_st      <= RX_FEED96_P1;
							else
								rx_ch_cnt  <= rx_ch_cnt + 1;
								play_raddr <= rx_rd_period & (rx_ch_cnt + 2);
							end if;
						else
							play_raddr <= rx_rd_period & to_unsigned(1, 6);
							rx_rd_d    <= '1';
						end if;

					when RX_FEED96_P1 =>
						-- Feed phase 1 (slots 24..47) into SRC decimator, then run
						if rx_rd_d = '1' then
							src_rx_we     <= '1';
							src_rx_phase  <= '1';
							src_rx_ch     <= rx_ch_cnt(4 downto 0);
							src_rx_sample <= play_rdata;
							if rx_ch_cnt = 23 then
								src_rx_run   <= '1';
								rx_rd_period <= rx_rd_period + 1;
								rd_inc       := true;
								rx_st        <= RX_IDLE;
							else
								rx_ch_cnt  <= rx_ch_cnt + 1;
								play_raddr <= rx_rd_period & (rx_ch_cnt + to_unsigned(26, 6));
							end if;
						else
							play_raddr <= rx_rd_period & to_unsigned(25, 6);
							rx_rd_d    <= '1';
						end if;
				end case;

				-- Update ring occupancy
				level_next := rx_ring_level;
				if wr_inc and level_next < 8 then
					level_next := level_next + 1;
				end if;
				if rd_inc and level_next > 0 then
					level_next := level_next - 1;
				end if;
				rx_ring_level <= level_next;
			end if;
		end if;
	end process;

	-- ================================================================
	-- 7. SPI Aux Byte FIFOs <-> Upstream AES50v2 Aux Encoder / Decoder
	-- ================================================================
	gen_aux : if G_ENABLE_AUX generate
		-- SPI TX byte FIFO (15-byte distributed RAM; drains at 3.125 Mbps into u_aux_enc's 1023-byte FIFO)
		u_spi_tx_fifo : entity work.aes50_fifo
			generic map (G_AW => 4, G_DW => 8)
			port map (
				clk_i => clk50_i, rst_i => rst_i,
				wr_en_i => aux_tx_we_i, wr_data_i => aux_tx_byte_i,
				rd_en_i => spi_tx_rd, rd_data_o => spi_tx_data,
				empty_o => spi_tx_empty, count_o => spi_tx_cnt, overflow_o => open
			);

		u_int_utx : entity work.aes50_uart_tx
			port map (
				clk_i => clk50_i,
				clks_per_bit_i => C_INT_UART_CLKS,
				start_i => int_utx_start, data_i => spi_tx_data,
				busy_o => int_utx_busy, tx_o => int_utx_line
			);

		u_aux_enc : entity work.aes50_aux_uart_enc
			generic map (G_FIFO_AW => G_AUX_FIFO_AW)
			port map (
				clk_i => clk50_i, rst_i => rst_i,
				uart_i => int_utx_line,
				clks_per_bit_i => C_INT_UART_CLKS,
				timeout_clks_i => C_INT_UART_TIMEOUT,
				fs48_i => '1',
				req_i => enc_req, req_page_i => enc_page,
				aux_we_o => enc_aux_we, aux_addr_o => enc_aux_addr, aux_data_o => enc_aux_data,
				done_o => enc_done, overflow_o => open
			);

		u_aux_page_ram : entity work.aes50_dpram
			generic map (G_AW => 8, G_DW => 16)
			port map (
				wclk_i => clk50_i, we_i => enc_aux_we, waddr_i => enc_aux_addr, wdata_i => enc_aux_data,
				rclk_i => clk50_i, raddr_i => aux_page_raddr, rdata_o => aux_page_rdata
			);

		-- aes50_par_core aux RX -> aes50_aux_uart_dec (31-byte distributed RAM) -> SPI RX byte FIFO
		u_aux_dec : entity work.aes50_aux_uart_dec
			generic map (G_FIFO_AW => 5)
			port map (
				clk_i => clk50_i, rst_i => rst_i,
				aux_we_i => dec_aux_we, aux_data_i => dec_aux_data, aux_start_i => dec_aux_start,
				clks_per_bit_i => C_INT_UART_CLKS,
				uart_o => dec_uart_line, overflow_o => open
			);

		u_int_urx : entity work.aes50_uart_rx
			port map (
				clk_i => clk50_i,
				clks_per_bit_i => C_INT_UART_CLKS,
				rx_i => dec_uart_line,
				valid_o => int_urx_valid, data_o => int_urx_byte
			);

		u_spi_rx_fifo : entity work.aes50_fifo
			generic map (G_AW => G_AUX_FIFO_AW, G_DW => 8)
			port map (
				clk_i => clk50_i, rst_i => rst_i,
				wr_en_i => int_urx_valid, wr_data_i => int_urx_byte,
				rd_en_i => aux_rx_rd_i, rd_data_o => aux_rx_byte_o,
				empty_o => spi_rx_empty, count_o => spi_rx_cnt, overflow_o => open
			);

		aux_tx_count_o <= std_logic_vector(resize(spi_tx_cnt, 16));
		aux_rx_count_o <= std_logic_vector(resize(spi_rx_cnt, 16));
		-- Mask valid for 1 clock after write-to-empty or pop so rd_data_o is always settled
		aux_rx_valid_o <= (not spi_rx_empty) and spi_rx_ne_d;

		-- Pump SPI TX FIFO -> internal UART TX, and pump encoder pages <-> aes50_par_core
		process (clk50_i)
		begin
			if rising_edge(clk50_i) then
				spi_tx_rd     <= '0';
				int_utx_start <= '0';
				enc_req       <= '0';
				aux_tx_we_c   <= '0';
				aux_rx_rd_c   <= '0';
				dec_aux_we    <= '0';

				if rst_i = '1' then
					int_utx_fetch <= "00";
					aux_drain_on  <= '0';
					aux_drain_d   <= '0';
					enc_page      <= '0';
					aux_rx_pipe   <= "00";
					aux_rx_pace   <= (others => '0');
					spi_rx_ne_d   <= '0';
				else
					spi_rx_ne_d <= (not spi_rx_empty) and (not aux_rx_rd_i);
					-- Drain SPI TX byte FIFO into internal UART TX (aes50_fifo is FWFT
					-- with 1-clock write-to-rd_data_o latency)
					case int_utx_fetch is
						when "00" =>
							if spi_tx_empty = '0' and int_utx_busy = '0' then
								int_utx_fetch <= "01";
							end if;
						when "01" =>
							int_utx_start <= '1';
							spi_tx_rd     <= '1';
							int_utx_fetch <= "10";
						when others =>
							if int_utx_busy = '1' then
								int_utx_fetch <= "00";
							end if;
					end case;

					-- On aux_tx_req from aes50_par_core, ask encoder for 44 words
					if aux_tx_req = '1' then
						enc_req <= '1';
					end if;
					if enc_done = '1' then
						aux_send_page  <= enc_page;
						enc_page       <= not enc_page;
						aux_drain_on   <= '1';
						aux_drain_idx  <= (others => '0');
						aux_drain_d    <= '0';
						aux_page_raddr <= enc_page & to_unsigned(0, 7);
					end if;
					if aux_drain_on = '1' then
						if aux_drain_d = '1' then
							aux_tx_we_c   <= '1';
							aux_tx_data_c <= aux_page_rdata;
							if aux_drain_idx = 43 then
								aux_drain_on <= '0';
								aux_drain_d  <= '0';
							else
								aux_drain_idx  <= aux_drain_idx + 1;
								aux_page_raddr <= aux_send_page & resize(aux_drain_idx + 2, 7);
							end if;
						else
							aux_page_raddr <= aux_send_page & to_unsigned(1, 7);
							aux_drain_d    <= '1';
						end if;
					end if;

					-- Drain aux RX words from aes50_par_core into aes50_aux_uart_dec
					-- Pace to 1 word / 32 clocks (28 us / block) so u_aux_dec's 15-word
					-- input FIFO (19 clocks / word) never overflows.
					if aux_rx_pipe(1) = '1' then
						dec_aux_we    <= '1';
						dec_aux_start <= aux_rx_first_c;
						dec_aux_data  <= aux_rx_data_c;
					end if;
					aux_rx_pipe(1) <= aux_rx_pipe(0);
					aux_rx_pipe(0) <= '0';
					if aux_rx_pace /= 0 then
						aux_rx_pace <= aux_rx_pace - 1;
					elsif aux_rx_cnt_c > 0 and aux_rx_pipe = "00" then
						aux_rx_rd_c    <= '1';
						aux_rx_pipe(0) <= '1';
						aux_rx_pace    <= to_unsigned(31, 5);
					end if;
				end if;
			end if;
		end process;
	end generate;

	gen_no_aux : if not G_ENABLE_AUX generate
		aux_tx_count_o <= (others => '0');
		aux_rx_byte_o  <= (others => '0');
		aux_rx_valid_o <= '0';
		aux_rx_count_o <= (others => '0');
	end generate;

end architecture rtl;
