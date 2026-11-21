-- ===========================================================================
-- OpenX32 - Wrapper for AES50 VHDL IP-CORE (v2)
--
-- Adapts YetAnotherElectronicsChannel/AES50v2 (aes50_top) to OpenX32:
--   * std_logic_vector ports for timer/config inputs (schematic & VHDL friendly)
--   * No clk100_i required (50 MHz RMII + 1024xfs + BCLK only)
--   * OpenX32-optimized default generics to minimize LUT/BRAM footprint
--     (critical when coexisting with OpenInferno on LFE5U-25F / XC3S1400A):
--       - G_SUPPORT_44K1 = false : 48 kHz only (halves TX/RX sample RAMs)
--       - G_UART_AUX     = true  : headamp/control tunnel over i.MX25 UART4
--       - G_TDM_AUX      = false : removes unused TDM aux RAMs
--       - G_UART_FIFO_AW = 10    : 1023-byte FIFOs (max OMC msg is 408 B)
--       - G_FSYNC_DELAY  = 1     : 1-BCLK delay matching OpenX32 TDM8 modules
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_v2_wrapper is
	generic (
		G_SUPPORT_44K1		: boolean := false;
		G_UART_AUX			: boolean := true;
		G_TDM_AUX			: boolean := false;
		G_UART_FIFO_AW		: natural := 10;
		G_FSYNC_DELAY		: natural := 1;
		G_TX_LEAD_ADJ		: integer := 0;
		G_SIM_ASSM_SAMPLES	: natural := 2048
	);
	port (
		-- clk and reset (50 MHz RMII clock only; clk100_i removed in v2)
		clk50_i								: in  std_logic;
		rst_i								: in  std_logic;

		-- samplerate and operation mode
		fs_mode_i							: in  std_logic_vector(1 downto 0);
		sys_mode_i							: in  std_logic_vector(1 downto 0);
		tdm8_i2s_mode_i						: in  std_logic;
		aux_tx_tdm_uart_select_i			: in  std_logic;

		-- connection to RMII PHY
		rmii_crs_dv_i						: in  std_logic;
		rmii_rxd_i							: in  std_logic_vector(1 downto 0);
		rmii_tx_en_o						: out std_logic;
		rmii_txd_o							: out std_logic_vector(1 downto 0);
		phy_rst_n_o							: out std_logic;

		-- connection to AES50 LVDS clock transceivers
		aes50_clk_a_rx_i					: in  std_logic;
		aes50_clk_a_tx_o					: out std_logic;
		aes50_clk_a_tx_en_o					: out std_logic;

		aes50_clk_b_rx_i					: in  std_logic;
		aes50_clk_b_tx_o					: out std_logic;
		aes50_clk_b_tx_en_o					: out std_logic;

		-- interface to external PLL (CS2000-CP)
		clk_1024xfs_from_pll_i				: in  std_logic;
		pll_lock_n_i						: in  std_logic;
		clk_to_pll_o						: out std_logic;
		pll_mult_value_o					: out std_logic_vector(31 downto 0);
		pll_init_busy_i						: in  std_logic;

		-- TDM / I2S interface (bclk_readback_i clocks the TDM/I2S SerDes in v2)
		mclk_o								: out std_logic;
		wclk_o								: out std_logic;
		bclk_o								: out std_logic;
		wclk_readback_i						: in  std_logic;
		bclk_readback_i						: in  std_logic;
		wclk_out_en_o						: out std_logic;
		bclk_out_en_o						: out std_logic;

		tdm_i								: in  std_logic_vector(6 downto 0);
		tdm_o								: out std_logic_vector(6 downto 0);

		i2s_i								: in  std_logic;
		i2s_o								: out std_logic;

		-- AES50 link status and debug
		aes_ok_o							: out std_logic;
		dbg_o								: out std_logic_vector(7 downto 0);

		-- UART aux tunnel
		uart_o								: out std_logic;
		uart_i								: in  std_logic;
		uart_overflow_o						: out std_logic;

		-- configuration constants (50 MHz clock ticks)
		debug_out_signal_pulse_len_i		: in  std_logic_vector(19 downto 0);
		first_transmit_start_counter_48k_i	: in  std_logic_vector(22 downto 0);
		first_transmit_start_counter_44k1_i	: in  std_logic_vector(22 downto 0);
		wd_aes_clk_timeout_i				: in  std_logic_vector(5 downto 0);
		wd_aes_rx_dv_timeout_i				: in  std_logic_vector(14 downto 0);
		mdix_timer_1ms_reference_i			: in  std_logic_vector(16 downto 0);
		aes_clk_ok_counter_reference_i		: in  std_logic_vector(19 downto 0);
		mult_clk625_48k_i					: in  std_logic_vector(31 downto 0);
		mult_clk625_44k1_i					: in  std_logic_vector(31 downto 0);
		uart_clks_per_bit_i					: in  std_logic_vector(9 downto 0);
		uart_timeout_clks_i					: in  std_logic_vector(19 downto 0)
	);
end entity aes50_v2_wrapper;

architecture rtl of aes50_v2_wrapper is
	signal pll_mult_nat						: natural;
	signal debug_pulse_len_nat				: natural range 0 to 1000000;
	signal first_tx_48k_nat					: natural range 0 to 5000000;
	signal first_tx_44k1_nat				: natural range 0 to 5000000;
	signal wd_clk_nat						: natural range 0 to 50;
	signal wd_rx_dv_nat						: natural range 0 to 20000;
	signal mdix_1ms_nat						: natural range 0 to 100000;
	signal clk_ok_ref_nat					: natural range 0 to 1000000;
	signal mult_48k_nat						: natural;
	signal mult_44k1_nat					: natural;
	signal uart_clks_nat					: natural range 0 to 65535;
	signal uart_timeout_nat					: natural range 0 to 16777215;
begin
	debug_pulse_len_nat	<= to_integer(unsigned(debug_out_signal_pulse_len_i));
	first_tx_48k_nat	<= to_integer(unsigned(first_transmit_start_counter_48k_i));
	first_tx_44k1_nat	<= to_integer(unsigned(first_transmit_start_counter_44k1_i));
	wd_clk_nat			<= to_integer(unsigned(wd_aes_clk_timeout_i));
	wd_rx_dv_nat		<= to_integer(unsigned(wd_aes_rx_dv_timeout_i));
	mdix_1ms_nat		<= to_integer(unsigned(mdix_timer_1ms_reference_i));
	clk_ok_ref_nat		<= to_integer(unsigned(aes_clk_ok_counter_reference_i));
	mult_48k_nat		<= to_integer(unsigned(mult_clk625_48k_i(30 downto 0)));
	mult_44k1_nat		<= to_integer(unsigned(mult_clk625_44k1_i(30 downto 0)));
	uart_clks_nat		<= to_integer(unsigned(uart_clks_per_bit_i));
	uart_timeout_nat	<= to_integer(unsigned(uart_timeout_clks_i));

	pll_mult_value_o	<= std_logic_vector(to_unsigned(pll_mult_nat, 32));

	u_aes50_top : entity work.aes50_top
		generic map (
			G_SUPPORT_44K1		=> G_SUPPORT_44K1,
			G_UART_AUX			=> G_UART_AUX,
			G_TDM_AUX			=> G_TDM_AUX,
			G_UART_FIFO_AW		=> G_UART_FIFO_AW,
			G_FSYNC_DELAY		=> G_FSYNC_DELAY,
			G_TX_LEAD_ADJ		=> G_TX_LEAD_ADJ,
			G_SIM_ASSM_SAMPLES	=> G_SIM_ASSM_SAMPLES
		)
		port map (
			clk50_i								=> clk50_i,
			rst_i								=> rst_i,
			fs_mode_i							=> fs_mode_i,
			sys_mode_i							=> sys_mode_i,
			tdm8_i2s_mode_i						=> tdm8_i2s_mode_i,
			aux_tx_tdm_uart_select_i			=> aux_tx_tdm_uart_select_i,
			rmii_crs_dv_i						=> rmii_crs_dv_i,
			rmii_rxd_i							=> rmii_rxd_i,
			rmii_tx_en_o						=> rmii_tx_en_o,
			rmii_txd_o							=> rmii_txd_o,
			phy_rst_n_o							=> phy_rst_n_o,
			aes50_clk_a_rx_i					=> aes50_clk_a_rx_i,
			aes50_clk_a_tx_o					=> aes50_clk_a_tx_o,
			aes50_clk_a_tx_en_o					=> aes50_clk_a_tx_en_o,
			aes50_clk_b_rx_i					=> aes50_clk_b_rx_i,
			aes50_clk_b_tx_o					=> aes50_clk_b_tx_o,
			aes50_clk_b_tx_en_o					=> aes50_clk_b_tx_en_o,
			clk_1024xfs_from_pll_i				=> clk_1024xfs_from_pll_i,
			pll_lock_n_i						=> pll_lock_n_i,
			clk_to_pll_o						=> clk_to_pll_o,
			pll_mult_value_o					=> pll_mult_nat,
			pll_init_busy_i						=> pll_init_busy_i,
			mclk_o								=> mclk_o,
			wclk_o								=> wclk_o,
			bclk_o								=> bclk_o,
			wclk_readback_i						=> wclk_readback_i,
			bclk_readback_i						=> bclk_readback_i,
			wclk_out_en_o						=> wclk_out_en_o,
			bclk_out_en_o						=> bclk_out_en_o,
			tdm_i								=> tdm_i,
			tdm_o								=> tdm_o,
			i2s_i								=> i2s_i,
			i2s_o								=> i2s_o,
			aes_ok_o							=> aes_ok_o,
			dbg_o								=> dbg_o,
			uart_o								=> uart_o,
			uart_i								=> uart_i,
			debug_out_signal_pulse_len_i		=> debug_pulse_len_nat,
			first_transmit_start_counter_48k_i	=> first_tx_48k_nat,
			first_transmit_start_counter_44k1_i	=> first_tx_44k1_nat,
			wd_aes_clk_timeout_i				=> wd_clk_nat,
			wd_aes_rx_dv_timeout_i				=> wd_rx_dv_nat,
			mdix_timer_1ms_reference_i			=> mdix_1ms_nat,
			aes_clk_ok_counter_reference_i		=> clk_ok_ref_nat,
			mult_clk625_48k_i					=> mult_48k_nat,
			mult_clk625_44k1_i					=> mult_44k1_nat,
			uart_clks_per_bit_i					=> uart_clks_nat,
			uart_timeout_clks_i					=> uart_timeout_nat,
			uart_overflow_o						=> uart_overflow_o
		);
end architecture rtl;
