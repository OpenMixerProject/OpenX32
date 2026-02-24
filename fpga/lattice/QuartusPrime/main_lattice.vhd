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
-- CREATED		"Tue Feb 24 22:31:56 2026"

LIBRARY ieee;
USE ieee.std_logic_1164.all; 

LIBRARY work;

ENTITY main_lattice IS 
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
		CARD_IN1 :  IN  STD_LOGIC;
		CARD_IN2 :  IN  STD_LOGIC;
		CARD_IN0 :  IN  STD_LOGIC;
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
END main_lattice;

ARCHITECTURE bdf_type OF main_lattice IS 

COMPONENT spi_rx_routing
	PORT(clk : IN STD_LOGIC;
		 i_spi_ncs : IN STD_LOGIC;
		 i_spi_clk : IN STD_LOGIC;
		 i_spi_data : IN STD_LOGIC;
		 o_cfg_wr_en : OUT STD_LOGIC;
		 o_cfg_wr_addr : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_cfg_wr_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT reset
	PORT(clk : IN STD_LOGIC;
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

COMPONENT aes50_rmii_rxd
	PORT(clkin : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 datain : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
		 sclk : OUT STD_LOGIC;
		 q : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
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

COMPONENT audiomatrix_routing_ram
GENERIC (ADDR_WIDTH : INTEGER;
			RAM_DEPTH : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 cfg_wr_en : IN STD_LOGIC;
		 cfg_wr_addr : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 cfg_wr_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 channel_idx : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 read_addr : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audioclk
	PORT(fs_x_1024_i : IN STD_LOGIC;
		 fs_x_512_o : OUT STD_LOGIC;
		 fs_x_256_o : OUT STD_LOGIC;
		 fs_o : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT lattice_pll_phy
	PORT(CLKI : IN STD_LOGIC;
		 RST : IN STD_LOGIC;
		 CLKOP : OUT STD_LOGIC;
		 CLKOS : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT debug_data
	PORT(clk : IN STD_LOGIC;
		 cfg_wr_en : IN STD_LOGIC;
		 cfg_wr_addr : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 cfg_wr_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 debugbits : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT clkinverter
	PORT(clk_i : IN STD_LOGIC;
		 inv_i : IN STD_LOGIC;
		 clk_o : OUT STD_LOGIC
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

COMPONENT tdm_demux
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 data_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch1_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch2_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch3_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch4_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch5_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch6_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch7_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch8_out : OUT STD_LOGIC_VECTOR(23 DOWNTO 0)
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

COMPONENT ultranet_tx
GENERIC (AES3_PREAMBLE_X : STD_LOGIC_VECTOR(7 DOWNTO 0);
			AES3_PREAMBLE_Y : STD_LOGIC_VECTOR(7 DOWNTO 0);
			AES3_PREAMBLE_Z : STD_LOGIC_VECTOR(7 DOWNTO 0);
			FRAME_COUNTER_RESET : STD_LOGIC_VECTOR(8 DOWNTO 0)
			);
	PORT(bit_clock : IN STD_LOGIC;
		 ch1 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch2 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch3 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch4 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch5 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch6 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch7 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch8 : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ultranet_out_p : OUT STD_LOGIC;
		 ultranet_out_m : OUT STD_LOGIC
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

COMPONENT audiomatrix_ram_write
GENERIC (ADDR_WIDTH : INTEGER;
			DATA_WIDTH : INTEGER;
			NUM_INPUT_PORTS : INTEGER;
			TDM_RECEIVER : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 input_data : IN STD_LOGIC_VECTOR(479 DOWNTO 0);
		 o_ram_wr_en : OUT STD_LOGIC;
		 o_ram_data : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 o_ram_write_addr : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audiomatrix_ram
GENERIC (ADDR_WIDTH : INTEGER;
			DATA_WIDTH : INTEGER;
			RAM_DEPTH : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 wr_en : IN STD_LOGIC;
		 i_data : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 read_addr : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 write_addr : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_data : OUT STD_LOGIC_VECTOR(23 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audiomatrix_ram_read
GENERIC (ADDR_WIDTH : INTEGER;
			DATA_WIDTH : INTEGER;
			NUM_OUTPUT_PORTS : INTEGER;
			TDM_SENDER : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 ram_data : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 channel_idx_out : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 data_out : OUT STD_LOGIC_VECTOR(479 DOWNTO 0)
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
		 wd_aes_clk_timeout : OUT STD_LOGIC_VECTOR(5 DOWNTO 0);
		 wd_aes_rx_dv_timeout : OUT STD_LOGIC_VECTOR(14 DOWNTO 0)
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

COMPONENT aes50_top
	PORT(clk50_i : IN STD_LOGIC;
		 clk100_i : IN STD_LOGIC;
		 rst_i : IN STD_LOGIC;
		 tdm8_i2s_mode_i : IN STD_LOGIC;
		 rmii_crs_dv_i : IN STD_LOGIC;
		 aes50_clk_a_rx_i : IN STD_LOGIC;
		 aes50_clk_b_rx_i : IN STD_LOGIC;
		 clk_1024xfs_from_pll_i : IN STD_LOGIC;
		 pll_lock_n_i : IN STD_LOGIC;
		 pll_init_busy_i : IN STD_LOGIC;
		 wclk_readback_i : IN STD_LOGIC;
		 bclk_readback_i : IN STD_LOGIC;
		 i2s_i : IN STD_LOGIC;
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
		 dbg_o : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 pll_mult_value_o : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 rmii_txd_o : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
		 tdm_o : OUT STD_LOGIC_VECTOR(6 DOWNTO 0)
	);
END COMPONENT;

COMPONENT lattice_pll
	PORT(CLKI : IN STD_LOGIC;
		 RST : IN STD_LOGIC;
		 CLKOP : OUT STD_LOGIC;
		 CLKOS : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT tdm_multichan_rx
GENERIC (DATA_WIDTH : INTEGER;
			TDM_RECEIVER : INTEGER
			);
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 tdm_in : IN STD_LOGIC_VECTOR(19 DOWNTO 0);
		 data_out : OUT STD_LOGIC_VECTOR(479 DOWNTO 0)
	);
END COMPONENT;

COMPONENT tdm_multichan_tx
GENERIC (DATA_WIDTH : INTEGER;
			TDM_RECEIVER : INTEGER
			);
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 data_in : IN STD_LOGIC_VECTOR(479 DOWNTO 0);
		 tdm_out : OUT STD_LOGIC_VECTOR(19 DOWNTO 0)
	);
END COMPONENT;

SIGNAL	aes50_bclk :  STD_LOGIC;
SIGNAL	aes50_fs_mode :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50_phy_clk :  STD_LOGIC_VECTOR(0 TO 0);
SIGNAL	aes50_phy_clk_data :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50_sys_mode :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50a_rmii_rxd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_rxd_in :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd_out :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_tdm_in :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	aes50a_tdm_out :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	audio_output :  STD_LOGIC_VECTOR(479 DOWNTO 0);
SIGNAL	clk_100MHz :  STD_LOGIC;
SIGNAL	clk_12_288MHz :  STD_LOGIC;
SIGNAL	clk_16MHz :  STD_LOGIC;
SIGNAL	clk_24_576MHz :  STD_LOGIC;
SIGNAL	clk_49_152MHz :  STD_LOGIC;
SIGNAL	clk_50MHz :  STD_LOGIC;
SIGNAL	debugbits :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	online :  STD_LOGIC;
SIGNAL	pll_locked :  STD_LOGIC;
SIGNAL	pripll_rst :  STD_LOGIC;
SIGNAL	rst :  STD_LOGIC;
SIGNAL	secpll_rst :  STD_LOGIC;
SIGNAL	tdm_fs :  STD_LOGIC;
SIGNAL	tdm_input :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	tdm_output :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	unusedClockA :  STD_LOGIC;
SIGNAL	unusedClockB :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_0 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_1 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_2 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_3 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_4 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_5 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_6 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_62 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_63 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_64 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_10 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_14 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_15 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_16 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_17 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_18 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_19 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_20 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_21 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_22 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_23 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_24 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_25 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_26 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_27 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_28 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_29 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_30 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_31 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_32 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_33 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_34 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_35 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_36 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_37 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_38 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_39 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_40 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_41 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_42 :  STD_LOGIC_VECTOR(479 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_43 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_44 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_45 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_46 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_47 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_48 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_49 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_50 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_51 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_52 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_53 :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_54 :  STD_LOGIC_VECTOR(19 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_55 :  STD_LOGIC_VECTOR(22 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_56 :  STD_LOGIC_VECTOR(22 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_57 :  STD_LOGIC_VECTOR(16 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_58 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_59 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_60 :  STD_LOGIC_VECTOR(5 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_61 :  STD_LOGIC_VECTOR(14 DOWNTO 0);


BEGIN 
imx25_uart4_rxd <= imx25_uart4_txd;
DA_TX <= imx25_uart3_txd;
AD0_TX <= imx25_uart3_txd;
AD1_TX <= imx25_uart3_txd;
CARD_TX <= imx25_uart3_txd;
SPI_MISO <= SPI_MOSI;
aes50a_clk_a_rx_nen_out <= '0';
aes50a_clk_b_rx_nen_out <= '0';
SYNTHESIZED_WIRE_49 <= '0';
SYNTHESIZED_WIRE_50 <= '0';
SYNTHESIZED_WIRE_51 <= '0';
SYNTHESIZED_WIRE_52 <= '0';



b2v_inst : spi_rx_routing
PORT MAP(clk => clk_24_576MHz,
		 i_spi_ncs => SPI_nCS0,
		 i_spi_clk => SPI_CLK,
		 i_spi_data => SPI_MOSI,
		 o_cfg_wr_en => SYNTHESIZED_WIRE_62,
		 o_cfg_wr_addr => SYNTHESIZED_WIRE_63,
		 o_cfg_wr_data => SYNTHESIZED_WIRE_64);


b2v_inst0 : reset
PORT MAP(clk => clk_16MHz,
		 o_pripll_rst => pripll_rst,
		 o_global_rst => rst,
		 o_online => online);


b2v_inst1 : cs2000cp_config
PORT MAP(clk => clk_16MHz,
		 i_start => pripll_rst,
		 i_txbusy => SYNTHESIZED_WIRE_0,
		 o_start => SYNTHESIZED_WIRE_14,
		 o_address => SYNTHESIZED_WIRE_15,
		 o_data => SYNTHESIZED_WIRE_16,
		 o_map => SYNTHESIZED_WIRE_17);


b2v_inst10 : aes50_rmii_rxd
PORT MAP(clkin => clk_50MHz,
		 reset => rst,
		 datain => aes50a_rmii_rxd_in,
		 q => aes50a_rmii_rxd);


b2v_inst11 : pcm1690dac_config
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_1,
		 i_txbusy => SYNTHESIZED_WIRE_2,
		 o_start => SYNTHESIZED_WIRE_18,
		 o_address => SYNTHESIZED_WIRE_19,
		 o_data => SYNTHESIZED_WIRE_20);


b2v_inst12 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_3,
		 in2 => SYNTHESIZED_WIRE_4,
		 output => AUX_CCLK);


b2v_inst13 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_5,
		 in2 => SYNTHESIZED_WIRE_6,
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


b2v_inst15 : audiomatrix_routing_ram
GENERIC MAP(ADDR_WIDTH => 8,
			RAM_DEPTH => 256
			)
PORT MAP(clk => clk_24_576MHz,
		 cfg_wr_en => SYNTHESIZED_WIRE_62,
		 cfg_wr_addr => SYNTHESIZED_WIRE_63,
		 cfg_wr_data => SYNTHESIZED_WIRE_64,
		 channel_idx => SYNTHESIZED_WIRE_10,
		 read_addr => SYNTHESIZED_WIRE_45);


b2v_inst16 : audioclk
PORT MAP(fs_x_1024_i => clk_49_152MHz,
		 fs_x_512_o => clk_24_576MHz,
		 fs_x_256_o => clk_12_288MHz,
		 fs_o => tdm_fs);


b2v_inst17 : lattice_pll_phy
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst,
		 CLKOS => clk_50MHz);


b2v_inst18 : debug_data
PORT MAP(clk => clk_24_576MHz,
		 cfg_wr_en => SYNTHESIZED_WIRE_62,
		 cfg_wr_addr => SYNTHESIZED_WIRE_63,
		 cfg_wr_data => SYNTHESIZED_WIRE_64,
		 debugbits => debugbits);


b2v_inst19 : clkinverter
PORT MAP(clk_i => clk_12_288MHz,
		 inv_i => debugbits(3),
		 clk_o => aes50_bclk);


b2v_inst2 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_14,
		 i_address => SYNTHESIZED_WIRE_15,
		 i_data => SYNTHESIZED_WIRE_16,
		 i_map => SYNTHESIZED_WIRE_17,
		 o_nCS => PLL_nCS,
		 o_cclk => PLL_CCLK,
		 o_cdata => PLL_CDATA,
		 o_busy => SYNTHESIZED_WIRE_0);


b2v_inst22 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(71 DOWNTO 48),
		 ch1_out => SYNTHESIZED_WIRE_21,
		 ch2_out => SYNTHESIZED_WIRE_22,
		 ch3_out => SYNTHESIZED_WIRE_23,
		 ch4_out => SYNTHESIZED_WIRE_24,
		 ch5_out => SYNTHESIZED_WIRE_25,
		 ch6_out => SYNTHESIZED_WIRE_26,
		 ch7_out => SYNTHESIZED_WIRE_27,
		 ch8_out => SYNTHESIZED_WIRE_28);


b2v_inst23 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(95 DOWNTO 72),
		 ch1_out => SYNTHESIZED_WIRE_33,
		 ch2_out => SYNTHESIZED_WIRE_34,
		 ch3_out => SYNTHESIZED_WIRE_35,
		 ch4_out => SYNTHESIZED_WIRE_36,
		 ch5_out => SYNTHESIZED_WIRE_37,
		 ch6_out => SYNTHESIZED_WIRE_38,
		 ch7_out => SYNTHESIZED_WIRE_39,
		 ch8_out => SYNTHESIZED_WIRE_40);


b2v_inst3 : spi_16bit_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_18,
		 i_address => SYNTHESIZED_WIRE_19,
		 i_data => SYNTHESIZED_WIRE_20,
		 o_nCS => AUX_DA_nCS,
		 o_cclk => SYNTHESIZED_WIRE_4,
		 o_cdata => SYNTHESIZED_WIRE_6,
		 o_busy => SYNTHESIZED_WIRE_2);


b2v_inst37 : aes50_rst
PORT MAP(clk100_i => clk_100MHz,
		 start_i => debugbits(0),
		 rst_o => SYNTHESIZED_WIRE_48);


b2v_inst39 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => SYNTHESIZED_WIRE_21,
		 ch2 => SYNTHESIZED_WIRE_22,
		 ch3 => SYNTHESIZED_WIRE_23,
		 ch4 => SYNTHESIZED_WIRE_24,
		 ch5 => SYNTHESIZED_WIRE_25,
		 ch6 => SYNTHESIZED_WIRE_26,
		 ch7 => SYNTHESIZED_WIRE_27,
		 ch8 => SYNTHESIZED_WIRE_28,
		 ultranet_out_p => P16_A_TXP,
		 ultranet_out_m => P16_A_TXM);


b2v_inst4 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_29,
		 i_address => SYNTHESIZED_WIRE_30,
		 i_data => SYNTHESIZED_WIRE_31,
		 i_map => SYNTHESIZED_WIRE_32,
		 o_nCS => AUX_AD_nCS,
		 o_cclk => SYNTHESIZED_WIRE_3,
		 o_cdata => SYNTHESIZED_WIRE_5,
		 o_busy => SYNTHESIZED_WIRE_41);


b2v_inst40 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => SYNTHESIZED_WIRE_33,
		 ch2 => SYNTHESIZED_WIRE_34,
		 ch3 => SYNTHESIZED_WIRE_35,
		 ch4 => SYNTHESIZED_WIRE_36,
		 ch5 => SYNTHESIZED_WIRE_37,
		 ch6 => SYNTHESIZED_WIRE_38,
		 ch7 => SYNTHESIZED_WIRE_39,
		 ch8 => SYNTHESIZED_WIRE_40,
		 ultranet_out_p => P16_B_TXP,
		 ultranet_out_m => P16_B_TXM);


b2v_inst42 : aes50_clk_ddr
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => debugbits(2 DOWNTO 1),
		 dout(0) => aes50_phy_clk(0));

aes50a_tdm_in(0) <= tdm_output(14);


aes50a_tdm_in(1) <= tdm_output(15);


aes50a_tdm_in(2) <= tdm_output(16);


aes50a_tdm_in(3) <= tdm_output(17);


aes50a_tdm_in(4) <= tdm_output(18);



b2v_inst5 : m8000adc_config
PORT MAP(clk => clk_16MHz,
		 i_start => online,
		 i_txbusy => SYNTHESIZED_WIRE_41,
		 o_start => SYNTHESIZED_WIRE_29,
		 o_done => SYNTHESIZED_WIRE_1,
		 o_address => SYNTHESIZED_WIRE_30,
		 o_data => SYNTHESIZED_WIRE_31,
		 o_map => SYNTHESIZED_WIRE_32);

aes50a_tdm_in(5) <= tdm_output(19);



b2v_inst52 : audiomatrix_ram_write
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			NUM_INPUT_PORTS => 160,
			TDM_RECEIVER => 20
			)
PORT MAP(clk => clk_24_576MHz,
		 bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 input_data => SYNTHESIZED_WIRE_42,
		 o_ram_wr_en => SYNTHESIZED_WIRE_43,
		 o_ram_data => SYNTHESIZED_WIRE_44,
		 o_ram_write_addr => SYNTHESIZED_WIRE_46);


b2v_inst53 : audiomatrix_ram
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			RAM_DEPTH => 256
			)
PORT MAP(clk => clk_24_576MHz,
		 wr_en => SYNTHESIZED_WIRE_43,
		 i_data => SYNTHESIZED_WIRE_44,
		 read_addr => SYNTHESIZED_WIRE_45,
		 write_addr => SYNTHESIZED_WIRE_46,
		 o_data => SYNTHESIZED_WIRE_47);


b2v_inst54 : audiomatrix_ram_read
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			NUM_OUTPUT_PORTS => 160,
			TDM_SENDER => 20
			)
PORT MAP(clk => clk_24_576MHz,
		 bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ram_data => SYNTHESIZED_WIRE_47,
		 channel_idx_out => SYNTHESIZED_WIRE_10,
		 data_out => audio_output);


b2v_inst55 : aes50_consts
PORT MAP(		 aes_clk_ok_counter_reference => SYNTHESIZED_WIRE_53,
		 debug_out_signal_pulse_len => SYNTHESIZED_WIRE_54,
		 first_transmit_start_counter_44k1 => SYNTHESIZED_WIRE_55,
		 first_transmit_start_counter_48k => SYNTHESIZED_WIRE_56,
		 mdix_timer_1ms_reference => SYNTHESIZED_WIRE_57,
		 mult_clk625_44k1 => SYNTHESIZED_WIRE_58,
		 mult_clk625_48k => SYNTHESIZED_WIRE_59,
		 wd_aes_clk_timeout => SYNTHESIZED_WIRE_60,
		 wd_aes_rx_dv_timeout => SYNTHESIZED_WIRE_61);

tdm_input(14) <= aes50a_tdm_out(0);


tdm_input(15) <= aes50a_tdm_out(1);


tdm_input(16) <= aes50a_tdm_out(2);


tdm_input(17) <= aes50a_tdm_out(3);



b2v_inst6 : aes50_rmii_txd
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => aes50a_rmii_txd,
		 dout => aes50a_rmii_txd_out);

tdm_input(18) <= aes50a_tdm_out(4);


tdm_input(19) <= aes50a_tdm_out(5);



b2v_inst62 : aes50_top
PORT MAP(clk50_i => clk_50MHz,
		 clk100_i => clk_100MHz,
		 rst_i => SYNTHESIZED_WIRE_48,
		 tdm8_i2s_mode_i => SYNTHESIZED_WIRE_49,
		 rmii_crs_dv_i => aes50a_rmii_rxd(0),
		 aes50_clk_a_rx_i => aes50a_clk_a_rx_in,
		 aes50_clk_b_rx_i => aes50a_clk_b_rx_in,
		 clk_1024xfs_from_pll_i => clk_49_152MHz,
		 pll_lock_n_i => SYNTHESIZED_WIRE_50,
		 pll_init_busy_i => SYNTHESIZED_WIRE_51,
		 wclk_readback_i => tdm_fs,
		 bclk_readback_i => aes50_bclk,
		 i2s_i => SYNTHESIZED_WIRE_52,
		 aes_clk_ok_counter_reference_i => SYNTHESIZED_WIRE_53,
		 debug_out_signal_pulse_len_i => SYNTHESIZED_WIRE_54,
		 first_transmit_start_counter_44k1_i => SYNTHESIZED_WIRE_55,
		 first_transmit_start_counter_48k_i => SYNTHESIZED_WIRE_56,
		 fs_mode_i => aes50_fs_mode,
		 mdix_timer_1ms_reference_i => SYNTHESIZED_WIRE_57,
		 mult_clk625_44k1_i => SYNTHESIZED_WIRE_58,
		 mult_clk625_48k_i => SYNTHESIZED_WIRE_59,
		 rmii_rxd_i => aes50a_rmii_rxd(2 DOWNTO 1),
		 sys_mode_i => debugbits(5 DOWNTO 4),
		 tdm_i => aes50a_tdm_in,
		 wd_aes_clk_timeout_i => SYNTHESIZED_WIRE_60,
		 wd_aes_rx_dv_timeout_i => SYNTHESIZED_WIRE_61,
		 rmii_tx_en_o => aes50a_rmii_txd(0),
		 phy_rst_n_o => aes50a_phy_rst_n_out,
		 aes50_clk_a_tx_o => aes50a_clk_a_tx_out,
		 aes50_clk_a_tx_en_o => aes50a_clk_a_tx_en_out,
		 aes50_clk_b_tx_o => aes50a_clk_b_tx_out,
		 aes50_clk_b_tx_en_o => aes50a_clk_b_tx_en_out,
		 rmii_txd_o => aes50a_rmii_txd(2 DOWNTO 1),
		 tdm_o => aes50a_tdm_out);











b2v_inst74 : lattice_pll
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst,
		 CLKOS => clk_100MHz);






b2v_inst8 : tdm_multichan_rx
GENERIC MAP(DATA_WIDTH => 24,
			TDM_RECEIVER => 20
			)
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 tdm_in => tdm_input,
		 data_out => SYNTHESIZED_WIRE_42);


b2v_inst9 : tdm_multichan_tx
GENERIC MAP(DATA_WIDTH => 24,
			TDM_RECEIVER => 20
			)
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output,
		 tdm_out => tdm_output);

clk_16MHz <= fpgaclk;
AD0_MCLK <= clk_12_288MHz;
clk_49_152MHz <= PLL_OUT;
AD0_BCLK <= clk_12_288MHz;
AD0_FSYNC <= tdm_fs;
AD1_MCLK <= clk_12_288MHz;
AD1_BCLK <= clk_12_288MHz;
AD1_FSYNC <= tdm_fs;
CARD_BCLK <= clk_12_288MHz;
CARD_FSYNC <= tdm_fs;
D_CLK <= clk_12_288MHz;
D_FS <= tdm_fs;
D_CLK2 <= clk_12_288MHz;
D_FS2 <= tdm_fs;
DA_MCLK <= clk_12_288MHz;
DA_BCLK <= clk_12_288MHz;
DA_FSYNC <= tdm_fs;
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
CARD_OUT0 <= tdm_output(4);
CARD_OUT1 <= tdm_output(5);
CARD_OUT2 <= tdm_output(6);
CARD_OUT3 <= tdm_output(7);
AUX_DA <= tdm_output(8);
DA_DATA1 <= tdm_output(0);
DA_DATA0 <= tdm_output(1);
aes50a_rmii_tx_en_out <= aes50a_rmii_txd_out(0);
aes50a_rmii_txd_1_out <= aes50a_rmii_txd_out(2);
aes50a_rmii_txd_0_out <= aes50a_rmii_txd_out(1);
aes50a_rmii_clk_out <= aes50_phy_clk(0);

aes50_fs_mode(0) <= '1';
aes50_fs_mode(1) <= '0';
aes50_phy_clk_data(1) <= '1';
aes50_phy_clk_data(0) <= '0';
aes50_sys_mode(1) <= '1';
aes50_sys_mode(0) <= '0';
aes50a_rmii_rxd_in(0) <= aes50a_rmii_crs_dv_in;
aes50a_rmii_rxd_in(1) <= aes50a_rmii_rxd_0_in;
aes50a_rmii_rxd_in(2) <= aes50a_rmii_rxd_1_in;
aes50a_tdm_in(6) <= '0';
tdm_input(0) <= AD0_DATA1;
tdm_input(1) <= AD0_DATA0;
tdm_input(2) <= AD1_DATA1;
tdm_input(3) <= AD1_DATA0;
tdm_input(4) <= CARD_IN3;
tdm_input(6) <= CARD_IN1;
tdm_input(5) <= CARD_IN2;
tdm_input(7) <= CARD_IN0;
tdm_input(9) <= DSP_DOUT0;
tdm_input(10) <= DSP_DOUT1;
tdm_input(11) <= DSP_DOUT2;
tdm_input(12) <= DSP_DOUT3;
tdm_input(13) <= DSP_DOUTAUX;
tdm_input(8) <= AUX_AD;
END bdf_type;