-- Copyright (C) 2025  Altera Corporation. All rights reserved.
-- Your use of Altera Corporation's design tools, logic functions 
-- and other software and tools, and any partner logic 
-- functions, and any output files from any of the foregoing 
-- (including device programming or simulation files), and any 
-- associated documentation or information are expressly subject 
-- to the terms and conditions of the Altera Program License 
-- Subscription Agreement, the Altera Quartus Prime License Agreement,
-- the Altera IP License Agreement, or other applicable license
-- agreement, including, without limitation, that your use is for
-- the sole purpose of programming logic devices manufactured by
-- Altera and sold by Altera or its authorized distributors.  Please
-- refer to the Altera Software License Subscription Agreements 
-- on the Quartus Prime software download page.

-- PROGRAM		"Quartus Prime"
-- VERSION		"Version 25.1std.0 Build 1129 10/21/2025 SC Lite Edition"
-- CREATED		"Sat Mar 14 21:01:19 2026"

LIBRARY ieee;
USE ieee.std_logic_1164.all; 

LIBRARY work;

ENTITY main_lattice_aes50only IS 
	PORT
	(
		fpgaclk :  IN  STD_LOGIC;
		imx25_uart4_txd :  IN  STD_LOGIC;
		DA_RX :  IN  STD_LOGIC;
		AD0_RX :  IN  STD_LOGIC;
		AD1_RX :  IN  STD_LOGIC;
		CARD_RX :  IN  STD_LOGIC;
		imx25_uart3_txd :  IN  STD_LOGIC;
		AD0_DATA1 :  IN  STD_LOGIC;
		AD0_DATA0 :  IN  STD_LOGIC;
		AD1_DATA1 :  IN  STD_LOGIC;
		AD1_DATA0 :  IN  STD_LOGIC;
		CARD_IN3 :  IN  STD_LOGIC;
		CARD_IN2 :  IN  STD_LOGIC;
		DSP_DOUT0 :  IN  STD_LOGIC;
		DSP_DOUT1 :  IN  STD_LOGIC;
		DSP_DOUT2 :  IN  STD_LOGIC;
		DSP_DOUT3 :  IN  STD_LOGIC;
		DSP_DOUTAUX :  IN  STD_LOGIC;
		AUX_AD :  IN  STD_LOGIC;
		SPI_nCS0 :  IN  STD_LOGIC;
		SPI_CLK :  IN  STD_LOGIC;
		SPI_MOSI :  IN  STD_LOGIC;
		PLL_OUT :  IN  STD_LOGIC;
		aes50a_rmii_rxd_1_in :  IN  STD_LOGIC;
		aes50a_rmii_rxd_0_in :  IN  STD_LOGIC;
		aes50a_rmii_crs_dv_in :  IN  STD_LOGIC;
		aes50a_clk_a_rx_in :  IN  STD_LOGIC;
		aes50a_clk_b_rx_in :  IN  STD_LOGIC;
		PLL_AUX :  IN  STD_LOGIC;
		CARD_RDY :  IN  STD_LOGIC;
		CARD_IN1 :  IN  STD_LOGIC;
		CARD_IN0 :  IN  STD_LOGIC;
		PLL_nCS :  OUT  STD_LOGIC;
		PLL_CCLK :  OUT  STD_LOGIC;
		PLL_CDATA :  OUT  STD_LOGIC;
		imx25_uart4_rxd :  OUT  STD_LOGIC;
		DA_TX :  OUT  STD_LOGIC;
		AD0_TX :  OUT  STD_LOGIC;
		AD1_TX :  OUT  STD_LOGIC;
		CARD_TX :  OUT  STD_LOGIC;
		imx25_uart3_rxd :  OUT  STD_LOGIC;
		AD0_MCLK :  OUT  STD_LOGIC;
		AD0_BCLK :  OUT  STD_LOGIC;
		AD0_FSYNC :  OUT  STD_LOGIC;
		AD1_MCLK :  OUT  STD_LOGIC;
		AD1_BCLK :  OUT  STD_LOGIC;
		AD1_FSYNC :  OUT  STD_LOGIC;
		CARD_BCLK :  OUT  STD_LOGIC;
		CARD_FSYNC :  OUT  STD_LOGIC;
		D_CLK :  OUT  STD_LOGIC;
		D_FS :  OUT  STD_LOGIC;
		D_CLK2 :  OUT  STD_LOGIC;
		D_FS2 :  OUT  STD_LOGIC;
		DA_MCLK :  OUT  STD_LOGIC;
		DA_BCLK :  OUT  STD_LOGIC;
		DA_FSYNC :  OUT  STD_LOGIC;
		AUX_MCLK :  OUT  STD_LOGIC;
		AUX_BCLK :  OUT  STD_LOGIC;
		AUX_FSYNC :  OUT  STD_LOGIC;
		AUX_CCLK :  OUT  STD_LOGIC;
		AUX_CDATA :  OUT  STD_LOGIC;
		AUX_nRST :  OUT  STD_LOGIC;
		AUX_AD_nCS :  OUT  STD_LOGIC;
		AUX_DA_nCS :  OUT  STD_LOGIC;
		DA_nRESET :  OUT  STD_LOGIC;
		AD_nRESET :  OUT  STD_LOGIC;
		PLL_IN :  OUT  STD_LOGIC;
		SPI_MISO :  OUT  STD_LOGIC;
		DSP_DIN0 :  OUT  STD_LOGIC;
		DSP_DIN1 :  OUT  STD_LOGIC;
		DSP_DIN2 :  OUT  STD_LOGIC;
		DSP_DIN3 :  OUT  STD_LOGIC;
		DSP_DINAUX :  OUT  STD_LOGIC;
		CARD_OUT0 :  OUT  STD_LOGIC;
		CARD_OUT1 :  OUT  STD_LOGIC;
		CARD_OUT2 :  OUT  STD_LOGIC;
		CARD_OUT3 :  OUT  STD_LOGIC;
		AUX_DA :  OUT  STD_LOGIC;
		DA_DATA1 :  OUT  STD_LOGIC;
		DA_DATA0 :  OUT  STD_LOGIC;
		P16_A_TXM :  OUT  STD_LOGIC;
		P16_A_TXP :  OUT  STD_LOGIC;
		P16_B_TXP :  OUT  STD_LOGIC;
		P16_B_TXM :  OUT  STD_LOGIC;
		aes50a_rmii_tx_en_out :  OUT  STD_LOGIC;
		aes50a_phy_rst_n_out :  OUT  STD_LOGIC;
		aes50a_clk_a_tx_en_out :  OUT  STD_LOGIC;
		aes50a_clk_a_tx_out :  OUT  STD_LOGIC;
		aes50a_clk_b_tx_out :  OUT  STD_LOGIC;
		aes50a_clk_b_tx_en_out :  OUT  STD_LOGIC;
		aes50a_rmii_txd_1_out :  OUT  STD_LOGIC;
		aes50a_rmii_txd_0_out :  OUT  STD_LOGIC;
		aes50a_clk_a_rx_nen_out :  OUT  STD_LOGIC;
		aes50a_clk_b_rx_nen_out :  OUT  STD_LOGIC;
		aes50a_rmii_clk_out :  OUT  STD_LOGIC
	);
END main_lattice_aes50only;

ARCHITECTURE bdf_type OF main_lattice_aes50only IS 

COMPONENT reset
	PORT(clk : IN STD_LOGIC;
		 pll_locked : IN STD_LOGIC;
		 o_pripll_rst : OUT STD_LOGIC;
		 o_secpll_rst : OUT STD_LOGIC;
		 o_global_rst : OUT STD_LOGIC;
		 o_online : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT cs2000cp_config
	PORT(clk : IN STD_LOGIC;
		 i_start : IN STD_LOGIC;
		 i_txbusy : IN STD_LOGIC;
		 o_start : OUT STD_LOGIC;
		 o_address : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_map : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT pcm1690dac_config
	PORT(clk : IN STD_LOGIC;
		 i_start : IN STD_LOGIC;
		 i_txbusy : IN STD_LOGIC;
		 o_start : OUT STD_LOGIC;
		 o_done : OUT STD_LOGIC;
		 o_address : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT or2_gate
	PORT(in1 : IN STD_LOGIC;
		 in2 : IN STD_LOGIC;
		 output : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT uart_collector
	PORT(clk_in : IN STD_LOGIC;
		 rst_in : IN STD_LOGIC;
		 uart1_in : IN STD_LOGIC;
		 uart2_in : IN STD_LOGIC;
		 uart3_in : IN STD_LOGIC;
		 uart4_in : IN STD_LOGIC;
		 card_rdy : IN STD_LOGIC;
		 uart_out : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT audioclk
	PORT(fs_x_1024_i : IN STD_LOGIC;
		 fs_x_512_o : OUT STD_LOGIC;
		 fs_x_256_o : OUT STD_LOGIC;
		 fs_o : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT spi_tx
	PORT(clk : IN STD_LOGIC;
		 i_start : IN STD_LOGIC;
		 i_address : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 i_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 i_map : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_nCS : OUT STD_LOGIC;
		 o_cclk : OUT STD_LOGIC;
		 o_cdata : OUT STD_LOGIC;
		 o_busy : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT spi_16bit_tx
	PORT(clk : IN STD_LOGIC;
		 i_start : IN STD_LOGIC;
		 i_address : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 i_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_nCS : OUT STD_LOGIC;
		 o_cclk : OUT STD_LOGIC;
		 o_cdata : OUT STD_LOGIC;
		 o_busy : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT aes50_rst
	PORT(clk100_i : IN STD_LOGIC;
		 start_i : IN STD_LOGIC;
		 rst_o : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT m8000adc_config
	PORT(clk : IN STD_LOGIC;
		 i_start : IN STD_LOGIC;
		 i_txbusy : IN STD_LOGIC;
		 o_start : OUT STD_LOGIC;
		 o_done : OUT STD_LOGIC;
		 o_address : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_map : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT aes50_consts
	PORT(		 aes_clk_ok_counter_reference : OUT STD_LOGIC_VECTOR(19 DOWNTO 0);
		 debug_out_signal_pulse_len : OUT STD_LOGIC_VECTOR(19 DOWNTO 0);
		 first_transmit_start_counter_44k1 : OUT STD_LOGIC_VECTOR(22 DOWNTO 0);
		 first_transmit_start_counter_48k : OUT STD_LOGIC_VECTOR(22 DOWNTO 0);
		 mdix_timer_1ms_reference : OUT STD_LOGIC_VECTOR(16 DOWNTO 0);
		 mult_clk625_44k1 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 mult_clk625_48k : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 uart_clks_per_bit : OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
		 uart_timeout_clks : OUT STD_LOGIC_VECTOR(19 DOWNTO 0);
		 wd_aes_clk_timeout : OUT STD_LOGIC_VECTOR(5 DOWNTO 0);
		 wd_aes_rx_dv_timeout : OUT STD_LOGIC_VECTOR(14 DOWNTO 0)
	);
END COMPONENT;

COMPONENT aes50_top
	PORT(clk50_i : IN STD_LOGIC;
		 clk100_i : IN STD_LOGIC;
		 rst_i : IN STD_LOGIC;
		 tdm8_i2s_mode_i : IN STD_LOGIC;
		 aux_tx_tdm_uart_select_i : IN STD_LOGIC;
		 rmii_crs_dv_i : IN STD_LOGIC;
		 aes50_clk_a_rx_i : IN STD_LOGIC;
		 aes50_clk_b_rx_i : IN STD_LOGIC;
		 clk_1024xfs_from_pll_i : IN STD_LOGIC;
		 pll_lock_n_i : IN STD_LOGIC;
		 pll_init_busy_i : IN STD_LOGIC;
		 wclk_readback_i : IN STD_LOGIC;
		 bclk_readback_i : IN STD_LOGIC;
		 i2s_i : IN STD_LOGIC;
		 uart_i : IN STD_LOGIC;
		 aes_clk_ok_counter_reference_i : IN STD_LOGIC_VECTOR(19 DOWNTO 0);
		 debug_out_signal_pulse_len_i : IN STD_LOGIC_VECTOR(19 DOWNTO 0);
		 first_transmit_start_counter_44k1_i : IN STD_LOGIC_VECTOR(22 DOWNTO 0);
		 first_transmit_start_counter_48k_i : IN STD_LOGIC_VECTOR(22 DOWNTO 0);
		 fs_mode_i : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 mdix_timer_1ms_reference_i : IN STD_LOGIC_VECTOR(16 DOWNTO 0);
		 mult_clk625_44k1_i : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		 mult_clk625_48k_i : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		 rmii_rxd_i : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 sys_mode_i : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 tdm_i : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
		 uart_clks_per_bit_i : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
		 uart_timeout_clks_i : IN STD_LOGIC_VECTOR(19 DOWNTO 0);
		 wd_aes_clk_timeout_i : IN STD_LOGIC_VECTOR(5 DOWNTO 0);
		 wd_aes_rx_dv_timeout_i : IN STD_LOGIC_VECTOR(14 DOWNTO 0);
		 rmii_tx_en_o : OUT STD_LOGIC;
		 phy_rst_n_o : OUT STD_LOGIC;
		 aes50_clk_a_tx_o : OUT STD_LOGIC;
		 aes50_clk_a_tx_en_o : OUT STD_LOGIC;
		 aes50_clk_b_tx_o : OUT STD_LOGIC;
		 aes50_clk_b_tx_en_o : OUT STD_LOGIC;
		 clk_to_pll_o : OUT STD_LOGIC;
		 mclk_o : OUT STD_LOGIC;
		 wclk_o : OUT STD_LOGIC;
		 bclk_o : OUT STD_LOGIC;
		 wclk_out_en_o : OUT STD_LOGIC;
		 bclk_out_en_o : OUT STD_LOGIC;
		 i2s_o : OUT STD_LOGIC;
		 aes_ok_o : OUT STD_LOGIC;
		 uart_o : OUT STD_LOGIC;
		 dbg_o : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 pll_mult_value_o : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 rmii_txd_o : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
		 tdm_o : OUT STD_LOGIC_VECTOR(6 DOWNTO 0)
	);
END COMPONENT;

COMPONENT aes50_clk_ddr
	PORT(refclk : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 data : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 clkout : OUT STD_LOGIC;
		 dout : OUT STD_LOGIC_VECTOR(0 TO 0)
	);
END COMPONENT;

COMPONENT lattice_pll
	PORT(CLKI : IN STD_LOGIC;
		 RST : IN STD_LOGIC;
		 CLKOP : OUT STD_LOGIC;
		 CLKOS : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT lattice_pll_phy
	PORT(CLKI : IN STD_LOGIC;
		 RST : IN STD_LOGIC;
		 CLKOP : OUT STD_LOGIC;
		 CLKOS : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT aes50_rmii_rxd
	PORT(clkin : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 datain : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
		 sclk : OUT STD_LOGIC;
		 q : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
	);
END COMPONENT;

COMPONENT aes50_rmii_txd
	PORT(refclk : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 data : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
		 clkout : OUT STD_LOGIC;
		 dout : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
	);
END COMPONENT;

SIGNAL	aes50_fs_mode :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50_phy_clk :  STD_LOGIC_VECTOR(0 TO 0);
SIGNAL	aes50_phy_clk_data :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50_sys_mode :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50_uart_txd :  STD_LOGIC;
SIGNAL	aes50a_rmii_rxd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_rxd_in :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd_out :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_tdm_in :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	aes50a_tdm_out :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	clk_100MHz :  STD_LOGIC;
SIGNAL	clk_12_288MHz :  STD_LOGIC;
SIGNAL	clk_16MHz :  STD_LOGIC;
SIGNAL	clk_24_576MHz :  STD_LOGIC;
SIGNAL	clk_49_152MHz :  STD_LOGIC;
SIGNAL	clk_50MHz :  STD_LOGIC;
SIGNAL	online :  STD_LOGIC;
SIGNAL	pll_locked :  STD_LOGIC;
SIGNAL	pripll_rst :  STD_LOGIC;
SIGNAL	rst :  STD_LOGIC;
SIGNAL	secpll_rst :  STD_LOGIC;
SIGNAL	tdm_fs :  STD_LOGIC;
SIGNAL	tdm_input0 :  STD_LOGIC;
SIGNAL	tdm_input1 :  STD_LOGIC;
SIGNAL	tdm_input10 :  STD_LOGIC;
SIGNAL	tdm_input11 :  STD_LOGIC;
SIGNAL	tdm_input12 :  STD_LOGIC;
SIGNAL	tdm_input13 :  STD_LOGIC;
SIGNAL	tdm_input2 :  STD_LOGIC;
SIGNAL	tdm_input3 :  STD_LOGIC;
SIGNAL	tdm_input8 :  STD_LOGIC;
SIGNAL	tdm_input9 :  STD_LOGIC;
SIGNAL	tdm_output :  STD_LOGIC_VECTOR(13 DOWNTO 8);
SIGNAL	unusedClockA :  STD_LOGIC;
SIGNAL	unusedClockB :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_0 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_1 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_2 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_3 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_4 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_5 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_6 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_7 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_8 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_9 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_10 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_11 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_12 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_13 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_14 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_15 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_16 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_17 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_18 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_19 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_20 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_21 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_22 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_23 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_24 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_25 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_26 :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_27 :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_28 :  STD_LOGIC_VECTOR(22 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_29 :  STD_LOGIC_VECTOR(22 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_30 :  STD_LOGIC_VECTOR(16 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_31 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_32 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_33 :  STD_LOGIC_VECTOR(9 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_34 :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_35 :  STD_LOGIC_VECTOR(5 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_36 :  STD_LOGIC_VECTOR(14 DOWNTO 0);


BEGIN 
DA_TX <= imx25_uart3_txd;
AD0_TX <= imx25_uart3_txd;
AD1_TX <= imx25_uart3_txd;
CARD_TX <= imx25_uart3_txd;
SPI_MISO <= SPI_MOSI;
aes50a_clk_a_rx_nen_out <= '0';
aes50a_clk_b_rx_nen_out <= '0';
SYNTHESIZED_WIRE_0 <= '1';
SYNTHESIZED_WIRE_21 <= '0';
SYNTHESIZED_WIRE_22 <= '1';
SYNTHESIZED_WIRE_23 <= '0';
SYNTHESIZED_WIRE_24 <= '0';
SYNTHESIZED_WIRE_25 <= '0';



b2v_inst0 : reset
PORT MAP(clk => clk_16MHz,
		 pll_locked => SYNTHESIZED_WIRE_0,
		 o_pripll_rst => pripll_rst,
		 o_global_rst => rst,
		 o_online => online);


b2v_inst1 : cs2000cp_config
PORT MAP(clk => clk_16MHz,
		 i_start => pripll_rst,
		 i_txbusy => SYNTHESIZED_WIRE_1,
		 o_start => SYNTHESIZED_WIRE_8,
		 o_address => SYNTHESIZED_WIRE_9,
		 o_data => SYNTHESIZED_WIRE_10,
		 o_map => SYNTHESIZED_WIRE_11);


b2v_inst11 : pcm1690dac_config
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_2,
		 i_txbusy => SYNTHESIZED_WIRE_3,
		 o_start => SYNTHESIZED_WIRE_12,
		 o_address => SYNTHESIZED_WIRE_13,
		 o_data => SYNTHESIZED_WIRE_14);


b2v_inst12 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_4,
		 in2 => SYNTHESIZED_WIRE_5,
		 output => AUX_CCLK);


b2v_inst13 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_6,
		 in2 => SYNTHESIZED_WIRE_7,
		 output => AUX_CDATA);


b2v_inst14 : uart_collector
PORT MAP(clk_in => clk_16MHz,
		 rst_in => rst,
		 uart1_in => DA_RX,
		 uart2_in => AD0_RX,
		 uart3_in => AD1_RX,
		 uart4_in => CARD_RX,
		 card_rdy => CARD_RDY,
		 uart_out => imx25_uart3_rxd);


b2v_inst16 : audioclk
PORT MAP(fs_x_1024_i => clk_49_152MHz,
		 fs_x_512_o => clk_24_576MHz,
		 fs_x_256_o => clk_12_288MHz,
		 fs_o => tdm_fs);


b2v_inst2 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_8,
		 i_address => SYNTHESIZED_WIRE_9,
		 i_data => SYNTHESIZED_WIRE_10,
		 i_map => SYNTHESIZED_WIRE_11,
		 o_nCS => PLL_nCS,
		 o_cclk => PLL_CCLK,
		 o_cdata => PLL_CDATA,
		 o_busy => SYNTHESIZED_WIRE_1);


b2v_inst3 : spi_16bit_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_12,
		 i_address => SYNTHESIZED_WIRE_13,
		 i_data => SYNTHESIZED_WIRE_14,
		 o_nCS => AUX_DA_nCS,
		 o_cclk => SYNTHESIZED_WIRE_5,
		 o_cdata => SYNTHESIZED_WIRE_7,
		 o_busy => SYNTHESIZED_WIRE_3);


b2v_inst37 : aes50_rst
PORT MAP(clk100_i => clk_100MHz,
		 start_i => online,
		 rst_o => SYNTHESIZED_WIRE_20);


b2v_inst4 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_15,
		 i_address => SYNTHESIZED_WIRE_16,
		 i_data => SYNTHESIZED_WIRE_17,
		 i_map => SYNTHESIZED_WIRE_18,
		 o_nCS => AUX_AD_nCS,
		 o_cclk => SYNTHESIZED_WIRE_4,
		 o_cdata => SYNTHESIZED_WIRE_6,
		 o_busy => SYNTHESIZED_WIRE_19);

aes50a_tdm_in(0) <= tdm_input0;


aes50a_tdm_in(1) <= tdm_input1;


aes50a_tdm_in(2) <= tdm_input2;


aes50a_tdm_in(3) <= tdm_input3;


aes50a_tdm_in(4) <= tdm_input0;



b2v_inst5 : m8000adc_config
PORT MAP(clk => clk_16MHz,
		 i_start => online,
		 i_txbusy => SYNTHESIZED_WIRE_19,
		 o_start => SYNTHESIZED_WIRE_15,
		 o_done => SYNTHESIZED_WIRE_2,
		 o_address => SYNTHESIZED_WIRE_16,
		 o_data => SYNTHESIZED_WIRE_17,
		 o_map => SYNTHESIZED_WIRE_18);

aes50a_tdm_in(5) <= tdm_input2;



b2v_inst55 : aes50_consts
PORT MAP(		 aes_clk_ok_counter_reference => SYNTHESIZED_WIRE_26,
		 debug_out_signal_pulse_len => SYNTHESIZED_WIRE_27,
		 first_transmit_start_counter_44k1 => SYNTHESIZED_WIRE_28,
		 first_transmit_start_counter_48k => SYNTHESIZED_WIRE_29,
		 mdix_timer_1ms_reference => SYNTHESIZED_WIRE_30,
		 mult_clk625_44k1 => SYNTHESIZED_WIRE_31,
		 mult_clk625_48k => SYNTHESIZED_WIRE_32,
		 uart_clks_per_bit => SYNTHESIZED_WIRE_33,
		 uart_timeout_clks => SYNTHESIZED_WIRE_34,
		 wd_aes_clk_timeout => SYNTHESIZED_WIRE_35,
		 wd_aes_rx_dv_timeout => SYNTHESIZED_WIRE_36);

tdm_output(8) <= aes50a_tdm_out(0);


tdm_output(9) <= aes50a_tdm_out(1);


tdm_output(10) <= aes50a_tdm_out(2);


tdm_output(11) <= aes50a_tdm_out(3);



tdm_output(12) <= aes50a_tdm_out(4);


tdm_output(13) <= aes50a_tdm_out(5);



b2v_inst62 : aes50_top
PORT MAP(clk50_i => clk_50MHz,
		 clk100_i => clk_100MHz,
		 rst_i => SYNTHESIZED_WIRE_20,
		 tdm8_i2s_mode_i => SYNTHESIZED_WIRE_21,
		 aux_tx_tdm_uart_select_i => SYNTHESIZED_WIRE_22,
		 rmii_crs_dv_i => aes50a_rmii_rxd(0),
		 aes50_clk_a_rx_i => aes50a_clk_a_rx_in,
		 aes50_clk_b_rx_i => aes50a_clk_b_rx_in,
		 clk_1024xfs_from_pll_i => clk_49_152MHz,
		 pll_lock_n_i => SYNTHESIZED_WIRE_23,
		 pll_init_busy_i => SYNTHESIZED_WIRE_24,
		 wclk_readback_i => tdm_fs,
		 bclk_readback_i => clk_12_288MHz,
		 i2s_i => SYNTHESIZED_WIRE_25,
		 uart_i => imx25_uart4_txd,
		 aes_clk_ok_counter_reference_i => SYNTHESIZED_WIRE_26,
		 debug_out_signal_pulse_len_i => SYNTHESIZED_WIRE_27,
		 first_transmit_start_counter_44k1_i => SYNTHESIZED_WIRE_28,
		 first_transmit_start_counter_48k_i => SYNTHESIZED_WIRE_29,
		 fs_mode_i => aes50_fs_mode,
		 mdix_timer_1ms_reference_i => SYNTHESIZED_WIRE_30,
		 mult_clk625_44k1_i => SYNTHESIZED_WIRE_31,
		 mult_clk625_48k_i => SYNTHESIZED_WIRE_32,
		 rmii_rxd_i => aes50a_rmii_rxd(2 DOWNTO 1),
		 sys_mode_i => aes50_sys_mode,
		 tdm_i => aes50a_tdm_in,
		 uart_clks_per_bit_i => SYNTHESIZED_WIRE_33,
		 uart_timeout_clks_i => SYNTHESIZED_WIRE_34,
		 wd_aes_clk_timeout_i => SYNTHESIZED_WIRE_35,
		 wd_aes_rx_dv_timeout_i => SYNTHESIZED_WIRE_36,
		 rmii_tx_en_o => aes50a_rmii_txd(0),
		 phy_rst_n_o => aes50a_phy_rst_n_out,
		 aes50_clk_a_tx_o => aes50a_clk_a_tx_out,
		 aes50_clk_a_tx_en_o => aes50a_clk_a_tx_en_out,
		 aes50_clk_b_tx_o => aes50a_clk_b_tx_out,
		 aes50_clk_b_tx_en_o => aes50a_clk_b_tx_en_out,
		 uart_o => aes50_uart_txd,
		 rmii_txd_o => aes50a_rmii_txd(2 DOWNTO 1),
		 tdm_o => aes50a_tdm_out);
















b2v_inst_aes50_clk : aes50_clk_ddr
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => aes50_phy_clk_data,
		 dout(0) => aes50_phy_clk(0));


b2v_inst_pll : lattice_pll
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst,
		 CLKOS => clk_100MHz);


b2v_inst_pll_phy : lattice_pll_phy
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst,
		 CLKOS => clk_50MHz);


b2v_inst_rmii_rxd : aes50_rmii_rxd
PORT MAP(clkin => clk_50MHz,
		 reset => rst,
		 datain => aes50a_rmii_rxd_in,
		 q => aes50a_rmii_rxd);


b2v_inst_rmii_txd : aes50_rmii_txd
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => aes50a_rmii_txd,
		 dout => aes50a_rmii_txd_out);

clk_16MHz <= fpgaclk;
imx25_uart4_rxd <= aes50_uart_txd;
clk_49_152MHz <= PLL_OUT;
tdm_input0 <= AD0_DATA1;
tdm_input1 <= AD0_DATA0;
tdm_input2 <= AD1_DATA1;
tdm_input3 <= AD1_DATA0;
AD0_MCLK <= clk_12_288MHz;
AD0_BCLK <= clk_12_288MHz;
AD0_FSYNC <= tdm_fs;
AD1_MCLK <= clk_12_288MHz;
AD1_BCLK <= clk_12_288MHz;
AD1_FSYNC <= tdm_fs;
D_CLK <= clk_12_288MHz;
D_FS <= tdm_fs;
D_CLK2 <= clk_24_576MHz;
D_FS2 <= tdm_fs;
AUX_MCLK <= clk_12_288MHz;
AUX_BCLK <= clk_12_288MHz;
AUX_FSYNC <= tdm_fs;
AUX_nRST <= online;
DA_nRESET <= online;
AD_nRESET <= online;
PLL_IN <= clk_16MHz;
DSP_DIN0 <= tdm_output(9);
DSP_DIN1 <= tdm_output(10);
DSP_DIN2 <= tdm_output(11);
DSP_DIN3 <= tdm_output(12);
DSP_DINAUX <= tdm_output(13);
CARD_OUT0 <= aes50_uart_txd;
AUX_DA <= tdm_output(8);
aes50a_rmii_tx_en_out <= aes50a_rmii_txd_out(0);
aes50a_rmii_txd_1_out <= aes50a_rmii_txd_out(2);
aes50a_rmii_txd_0_out <= aes50a_rmii_txd_out(1);
aes50a_rmii_clk_out <= aes50_phy_clk(0);

aes50_fs_mode(0) <= '1';
aes50_fs_mode(1) <= '0';
aes50_phy_clk_data(0) <= '0';
aes50_phy_clk_data(1) <= '1';
aes50_sys_mode(1) <= '1';
aes50_sys_mode(0) <= '0';
aes50a_rmii_rxd_in(0) <= aes50a_rmii_crs_dv_in;
aes50a_rmii_rxd_in(1) <= aes50a_rmii_rxd_0_in;
aes50a_rmii_rxd_in(2) <= aes50a_rmii_rxd_1_in;
aes50a_tdm_in(6) <= '0';
END bdf_type;