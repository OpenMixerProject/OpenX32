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
-- CREATED		"Sun Jun 28 00:37:49 2026"

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
		aes50a_rmii_er_in :  IN  STD_LOGIC;
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

COMPONENT reverse_mac
	PORT(mac_address_i : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
		 mac_address_o : OUT STD_LOGIC_VECTOR(47 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audioclk
	PORT(fs_x_1024_i : IN STD_LOGIC;
		 fs_x_512_o : OUT STD_LOGIC;
		 fs_x_256_o : OUT STD_LOGIC;
		 fs_o : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT ethernet_reset
	PORT(clk : IN STD_LOGIC;
		 power_good : IN STD_LOGIC;
		 phy_rstn : OUT STD_LOGIC;
		 mac_rst : OUT STD_LOGIC;
		 sendArpRequest : OUT STD_LOGIC;
		 tx_online : OUT STD_LOGIC
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

COMPONENT ace_audio_packet
	PORT(tx_clk : IN STD_LOGIC;
		 tx_busy : IN STD_LOGIC;
		 tx_byte_sent : IN STD_LOGIC;
		 audio_ready : IN STD_LOGIC;
		 audio_in : IN STD_LOGIC_VECTOR(1559 DOWNTO 0);
		 src_mac_address : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
		 tx_enable : OUT STD_LOGIC;
		 ace_tx_busy : OUT STD_LOGIC;
		 tx_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
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

COMPONENT aes50_rmii_rxd
	PORT(clkin : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 datain : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
		 sclk : OUT STD_LOGIC;
		 q : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
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

COMPONENT aes50_rmii_txd
	PORT(refclk : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 data : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
		 clkout : OUT STD_LOGIC;
		 dout : OUT STD_LOGIC_VECTOR(2 DOWNTO 0)
	);
END COMPONENT;

COMPONENT tdm_ace_demux
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 ace_tx_busy : IN STD_LOGIC;
		 tdm0_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm1_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm2_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm3_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm4_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm5_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm6_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 tdm7_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ready : OUT STD_LOGIC;
		 audio_out : OUT STD_LOGIC_VECTOR(1559 DOWNTO 0)
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

COMPONENT const_eth_config
GENERIC (DST_IP0 : INTEGER;
			DST_IP1 : INTEGER;
			DST_IP2 : INTEGER;
			DST_IP3 : INTEGER;
			DST_MAC0 : INTEGER;
			DST_MAC1 : INTEGER;
			DST_MAC2 : INTEGER;
			DST_MAC3 : INTEGER;
			DST_MAC4 : INTEGER;
			DST_MAC5 : INTEGER;
			DST_PORT : INTEGER;
			SRC_IP0 : INTEGER;
			SRC_IP1 : INTEGER;
			SRC_IP2 : INTEGER;
			SRC_IP3 : INTEGER;
			SRC_MAC0 : INTEGER;
			SRC_MAC1 : INTEGER;
			SRC_MAC2 : INTEGER;
			SRC_MAC3 : INTEGER;
			SRC_MAC4 : INTEGER;
			SRC_MAC5 : INTEGER;
			SRC_PORT : INTEGER
			);
	PORT(		 dst_ip_address : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 dst_mac_address : OUT STD_LOGIC_VECTOR(47 DOWNTO 0);
		 dst_udp_port : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		 src_ip_address : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
		 src_mac_address : OUT STD_LOGIC_VECTOR(47 DOWNTO 0);
		 src_udp_port : OUT STD_LOGIC_VECTOR(15 DOWNTO 0)
	);
END COMPONENT;

COMPONENT clk_by_x
GENERIC (divider : INTEGER
			);
	PORT(clk_in : IN STD_LOGIC;
		 clk_out : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT ethernet_rmii
GENERIC (MIIM_CLOCK_DIVIDER : INTEGER;
			MIIM_DISABLE : BOOLEAN;
			MIIM_PHY_ADDRESS : STD_LOGIC_VECTOR(4 DOWNTO 0);
			MIIM_POLL_WAIT_TICKS : INTEGER;
			MIIM_RESET_WAIT_TICKS : INTEGER
			);
	PORT(reset_i : IN STD_LOGIC;
		 rmii_clk_i : IN STD_LOGIC;
		 rmii_rx_er_i : IN STD_LOGIC;
		 rmii_rx_crs_dv_i : IN STD_LOGIC;
		 miim_clock_i : IN STD_LOGIC;
		 tx_enable_i : IN STD_LOGIC;
		 mdio_io : INOUT STD_LOGIC;
		 mac_address_i : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
		 rmii_rxd_i : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 tx_data_i : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 reset_o : OUT STD_LOGIC;
		 rmii_tx_er_o : OUT STD_LOGIC;
		 rmii_tx_en_o : OUT STD_LOGIC;
		 mdc_o : OUT STD_LOGIC;
		 link_up_o : OUT STD_LOGIC;
		 tx_clock_o : OUT STD_LOGIC;
		 tx_reset_o : OUT STD_LOGIC;
		 tx_byte_sent_o : OUT STD_LOGIC;
		 tx_busy_o : OUT STD_LOGIC;
		 rx_clock_o : OUT STD_LOGIC;
		 rx_reset_o : OUT STD_LOGIC;
		 rx_frame_o : OUT STD_LOGIC;
		 rx_byte_received_o : OUT STD_LOGIC;
		 rx_error_o : OUT STD_LOGIC;
		 rmii_txd_o : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
		 rx_data_o : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT udp_audio_packet
	PORT(tx_clk : IN STD_LOGIC;
		 tx_busy : IN STD_LOGIC;
		 tx_byte_sent : IN STD_LOGIC;
		 audio_sync : IN STD_LOGIC;
		 audio_ch0_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch10_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch11_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch12_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch13_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch14_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch15_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch1_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch2_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch3_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch4_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch5_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch6_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch7_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch8_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 audio_ch9_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 dst_ip_address : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		 dst_mac_address : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
		 dst_udp_port : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
		 src_ip_address : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		 src_mac_address : IN STD_LOGIC_VECTOR(47 DOWNTO 0);
		 src_udp_port : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
		 tx_enable : OUT STD_LOGIC;
		 tx_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
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

COMPONENT aes50_clk_ddr
	PORT(refclk : IN STD_LOGIC;
		 reset : IN STD_LOGIC;
		 data : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		 clkout : OUT STD_LOGIC;
		 dout : OUT STD_LOGIC_VECTOR(0 TO 0)
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

COMPONENT config_rxd
	PORT(clk : IN STD_LOGIC;
		 cfg_wr_en : IN STD_LOGIC;
		 cfg_wr_addr : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 cfg_wr_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 config_bits : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
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

SIGNAL	aes50_phy_clk :  STD_LOGIC_VECTOR(0 TO 0);
SIGNAL	aes50_phy_clk_data :  STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL	aes50a_rmii_rxd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_rxd_in :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	aes50a_rmii_txd_out :  STD_LOGIC_VECTOR(2 DOWNTO 0);
SIGNAL	audio_output :  STD_LOGIC_VECTOR(527 DOWNTO 0);
SIGNAL	clk_100MHz :  STD_LOGIC;
SIGNAL	clk_12_288MHz :  STD_LOGIC;
SIGNAL	clk_16MHz :  STD_LOGIC;
SIGNAL	clk_24_576MHz :  STD_LOGIC;
SIGNAL	clk_49_152MHz :  STD_LOGIC;
SIGNAL	clk_50MHz :  STD_LOGIC;
SIGNAL	configbits :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	mac_tx_busy :  STD_LOGIC;
SIGNAL	mac_tx_byte_sent :  STD_LOGIC;
SIGNAL	mac_tx_clk :  STD_LOGIC;
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
SIGNAL	SYNTHESIZED_WIRE_84 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_85 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_86 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_10 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_87 :  STD_LOGIC_VECTOR(47 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_12 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_13 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_14 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_15 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_16 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_17 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_18 :  STD_LOGIC_VECTOR(1559 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_20 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_21 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_22 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_23 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_24 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_25 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_26 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_27 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_28 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_29 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_30 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_31 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_32 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_33 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_34 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_35 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_36 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_37 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_38 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_39 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_40 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_41 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_42 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_43 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_44 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_45 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_46 :  STD_LOGIC_VECTOR(47 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_47 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_48 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_49 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_50 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_51 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_52 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_53 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_54 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_55 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_56 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_57 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_58 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_59 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_60 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_61 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_62 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_63 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_64 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_65 :  STD_LOGIC_VECTOR(47 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_66 :  STD_LOGIC_VECTOR(15 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_67 :  STD_LOGIC_VECTOR(31 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_69 :  STD_LOGIC_VECTOR(15 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_70 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_71 :  STD_LOGIC_VECTOR(479 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_72 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_73 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_74 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_75 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_76 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_88 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_83 :  STD_LOGIC;


BEGIN 
DA_TX <= imx25_uart3_txd;
AD0_TX <= imx25_uart3_txd;
AD1_TX <= imx25_uart3_txd;
CARD_TX <= imx25_uart3_txd;
SPI_MISO <= SPI_MOSI;
aes50a_clk_a_tx_en_out <= '0';
aes50a_clk_a_tx_out <= '0';
aes50a_clk_b_tx_out <= '0';
aes50a_clk_b_tx_en_out <= '0';
aes50a_clk_a_rx_nen_out <= '1';
aes50a_clk_b_rx_nen_out <= '1';



b2v_inst : spi_rx_routing
PORT MAP(clk => clk_24_576MHz,
		 i_spi_ncs => SPI_nCS0,
		 i_spi_clk => SPI_CLK,
		 i_spi_data => SPI_MOSI,
		 o_cfg_wr_en => SYNTHESIZED_WIRE_84,
		 o_cfg_wr_addr => SYNTHESIZED_WIRE_85,
		 o_cfg_wr_data => SYNTHESIZED_WIRE_86);


b2v_inst0 : reset
PORT MAP(clk => clk_16MHz,
		 o_pripll_rst => pripll_rst,
		 o_global_rst => rst,
		 o_online => online);


b2v_inst1 : cs2000cp_config
PORT MAP(clk => clk_16MHz,
		 i_start => pripll_rst,
		 i_txbusy => SYNTHESIZED_WIRE_0,
		 o_start => SYNTHESIZED_WIRE_13,
		 o_address => SYNTHESIZED_WIRE_14,
		 o_data => SYNTHESIZED_WIRE_15,
		 o_map => SYNTHESIZED_WIRE_16);


b2v_inst11 : pcm1690dac_config
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_1,
		 i_txbusy => SYNTHESIZED_WIRE_2,
		 o_start => SYNTHESIZED_WIRE_20,
		 o_address => SYNTHESIZED_WIRE_21,
		 o_data => SYNTHESIZED_WIRE_22);


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
		 cfg_wr_en => SYNTHESIZED_WIRE_84,
		 cfg_wr_addr => SYNTHESIZED_WIRE_85,
		 cfg_wr_data => SYNTHESIZED_WIRE_86,
		 channel_idx => SYNTHESIZED_WIRE_10,
		 read_addr => SYNTHESIZED_WIRE_74);


b2v_inst16 : reverse_mac
PORT MAP(mac_address_i => SYNTHESIZED_WIRE_87,
		 mac_address_o => SYNTHESIZED_WIRE_46);


b2v_inst17 : audioclk
PORT MAP(fs_x_1024_i => clk_49_152MHz,
		 fs_x_512_o => clk_24_576MHz,
		 fs_x_256_o => clk_12_288MHz,
		 fs_o => tdm_fs);


b2v_inst19 : ethernet_reset
PORT MAP(clk => SYNTHESIZED_WIRE_12,
		 power_good => online,
		 phy_rstn => aes50a_phy_rst_n_out,
		 mac_rst => SYNTHESIZED_WIRE_44);


b2v_inst2 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_13,
		 i_address => SYNTHESIZED_WIRE_14,
		 i_data => SYNTHESIZED_WIRE_15,
		 i_map => SYNTHESIZED_WIRE_16,
		 o_nCS => PLL_nCS,
		 o_cclk => PLL_CCLK,
		 o_cdata => PLL_CDATA,
		 o_busy => SYNTHESIZED_WIRE_0);


b2v_inst20 : ace_audio_packet
PORT MAP(tx_clk => mac_tx_clk,
		 tx_busy => mac_tx_busy,
		 tx_byte_sent => mac_tx_byte_sent,
		 audio_ready => SYNTHESIZED_WIRE_17,
		 audio_in => SYNTHESIZED_WIRE_18,
		 src_mac_address => SYNTHESIZED_WIRE_87,
		 tx_enable => SYNTHESIZED_WIRE_45,
		 ace_tx_busy => SYNTHESIZED_WIRE_23,
		 tx_data => SYNTHESIZED_WIRE_47);



b2v_inst22 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(71 DOWNTO 48),
		 ch1_out => SYNTHESIZED_WIRE_24,
		 ch2_out => SYNTHESIZED_WIRE_25,
		 ch3_out => SYNTHESIZED_WIRE_26,
		 ch4_out => SYNTHESIZED_WIRE_27,
		 ch5_out => SYNTHESIZED_WIRE_28,
		 ch6_out => SYNTHESIZED_WIRE_29,
		 ch7_out => SYNTHESIZED_WIRE_30,
		 ch8_out => SYNTHESIZED_WIRE_31);


b2v_inst23 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(95 DOWNTO 72),
		 ch1_out => SYNTHESIZED_WIRE_36,
		 ch2_out => SYNTHESIZED_WIRE_37,
		 ch3_out => SYNTHESIZED_WIRE_38,
		 ch4_out => SYNTHESIZED_WIRE_39,
		 ch5_out => SYNTHESIZED_WIRE_40,
		 ch6_out => SYNTHESIZED_WIRE_41,
		 ch7_out => SYNTHESIZED_WIRE_42,
		 ch8_out => SYNTHESIZED_WIRE_43);


b2v_inst24 : aes50_rmii_rxd
PORT MAP(clkin => clk_50MHz,
		 reset => rst,
		 datain => aes50a_rmii_rxd_in,
		 q => aes50a_rmii_rxd);


b2v_inst25 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(71 DOWNTO 48),
		 ch1_out => SYNTHESIZED_WIRE_48,
		 ch2_out => SYNTHESIZED_WIRE_55,
		 ch3_out => SYNTHESIZED_WIRE_56,
		 ch4_out => SYNTHESIZED_WIRE_57,
		 ch5_out => SYNTHESIZED_WIRE_58,
		 ch6_out => SYNTHESIZED_WIRE_59,
		 ch7_out => SYNTHESIZED_WIRE_60,
		 ch8_out => SYNTHESIZED_WIRE_61);


b2v_inst26 : tdm_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(95 DOWNTO 72),
		 ch1_out => SYNTHESIZED_WIRE_62,
		 ch2_out => SYNTHESIZED_WIRE_63,
		 ch3_out => SYNTHESIZED_WIRE_49,
		 ch4_out => SYNTHESIZED_WIRE_50,
		 ch5_out => SYNTHESIZED_WIRE_51,
		 ch6_out => SYNTHESIZED_WIRE_52,
		 ch7_out => SYNTHESIZED_WIRE_53,
		 ch8_out => SYNTHESIZED_WIRE_54);


b2v_inst3 : spi_16bit_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_20,
		 i_address => SYNTHESIZED_WIRE_21,
		 i_data => SYNTHESIZED_WIRE_22,
		 o_nCS => AUX_DA_nCS,
		 o_cclk => SYNTHESIZED_WIRE_4,
		 o_cdata => SYNTHESIZED_WIRE_6,
		 o_busy => SYNTHESIZED_WIRE_2);


b2v_inst34 : aes50_rmii_txd
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => aes50a_rmii_txd,
		 dout => aes50a_rmii_txd_out);


b2v_inst38 : tdm_ace_demux
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ace_tx_busy => SYNTHESIZED_WIRE_23,
		 tdm0_in => audio_output(359 DOWNTO 336),
		 tdm1_in => audio_output(383 DOWNTO 360),
		 tdm2_in => audio_output(407 DOWNTO 384),
		 tdm3_in => audio_output(431 DOWNTO 408),
		 tdm4_in => audio_output(455 DOWNTO 432),
		 tdm5_in => audio_output(479 DOWNTO 456),
		 tdm6_in => audio_output(503 DOWNTO 480),
		 tdm7_in => audio_output(527 DOWNTO 504),
		 audio_ready => SYNTHESIZED_WIRE_17,
		 audio_out => SYNTHESIZED_WIRE_18);


b2v_inst39 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => SYNTHESIZED_WIRE_24,
		 ch2 => SYNTHESIZED_WIRE_25,
		 ch3 => SYNTHESIZED_WIRE_26,
		 ch4 => SYNTHESIZED_WIRE_27,
		 ch5 => SYNTHESIZED_WIRE_28,
		 ch6 => SYNTHESIZED_WIRE_29,
		 ch7 => SYNTHESIZED_WIRE_30,
		 ch8 => SYNTHESIZED_WIRE_31,
		 ultranet_out_p => P16_A_TXP,
		 ultranet_out_m => P16_A_TXM);


b2v_inst4 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_32,
		 i_address => SYNTHESIZED_WIRE_33,
		 i_data => SYNTHESIZED_WIRE_34,
		 i_map => SYNTHESIZED_WIRE_35,
		 o_nCS => AUX_AD_nCS,
		 o_cclk => SYNTHESIZED_WIRE_3,
		 o_cdata => SYNTHESIZED_WIRE_5,
		 o_busy => SYNTHESIZED_WIRE_70);


b2v_inst40 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => SYNTHESIZED_WIRE_36,
		 ch2 => SYNTHESIZED_WIRE_37,
		 ch3 => SYNTHESIZED_WIRE_38,
		 ch4 => SYNTHESIZED_WIRE_39,
		 ch5 => SYNTHESIZED_WIRE_40,
		 ch6 => SYNTHESIZED_WIRE_41,
		 ch7 => SYNTHESIZED_WIRE_42,
		 ch8 => SYNTHESIZED_WIRE_43,
		 ultranet_out_p => P16_B_TXP,
		 ultranet_out_m => P16_B_TXM);


b2v_inst41 : const_eth_config
GENERIC MAP(DST_IP0 => 192,
			DST_IP1 => 168,
			DST_IP2 => 50,
			DST_IP3 => 98,
			DST_MAC0 => 0,
			DST_MAC1 => 36,
			DST_MAC2 => 155,
			DST_MAC3 => 127,
			DST_MAC4 => 185,
			DST_MAC5 => 42,
			DST_PORT => 4023,
			SRC_IP0 => 192,
			SRC_IP1 => 168,
			SRC_IP2 => 50,
			SRC_IP3 => 99,
			SRC_MAC0 => 0,
			SRC_MAC1 => 28,
			SRC_MAC2 => 35,
			SRC_MAC3 => 23,
			SRC_MAC4 => 74,
			SRC_MAC5 => 203,
			SRC_PORT => 4023
			)
PORT MAP(		 dst_ip_address => SYNTHESIZED_WIRE_64,
		 dst_mac_address => SYNTHESIZED_WIRE_65,
		 dst_udp_port => SYNTHESIZED_WIRE_66,
		 src_ip_address => SYNTHESIZED_WIRE_67,
		 src_mac_address => SYNTHESIZED_WIRE_87,
		 src_udp_port => SYNTHESIZED_WIRE_69);


b2v_inst43 : clk_by_x
GENERIC MAP(divider => 16000
			)
PORT MAP(clk_in => clk_16MHz,
		 clk_out => SYNTHESIZED_WIRE_12);


b2v_inst44 : ethernet_rmii
GENERIC MAP(MIIM_CLOCK_DIVIDER => 50,
			MIIM_DISABLE => false,
			MIIM_PHY_ADDRESS => "00000",
			MIIM_POLL_WAIT_TICKS => 10000000,
			MIIM_RESET_WAIT_TICKS => 0
			)
PORT MAP(reset_i => SYNTHESIZED_WIRE_44,
		 rmii_clk_i => clk_50MHz,
		 rmii_rx_er_i => aes50a_rmii_er_in,
		 rmii_rx_crs_dv_i => aes50a_rmii_rxd(0),
		 miim_clock_i => clk_50MHz,
		 tx_enable_i => SYNTHESIZED_WIRE_45,
		 mac_address_i => SYNTHESIZED_WIRE_46,
		 rmii_rxd_i => aes50a_rmii_rxd(2 DOWNTO 1),
		 tx_data_i => SYNTHESIZED_WIRE_47,
		 rmii_tx_en_o => aes50a_rmii_txd(0),
		 tx_clock_o => mac_tx_clk,
		 tx_byte_sent_o => mac_tx_byte_sent,
		 tx_busy_o => mac_tx_busy,
		 rmii_txd_o => aes50a_rmii_txd(2 DOWNTO 1));


b2v_inst45 : udp_audio_packet
PORT MAP(tx_clk => mac_tx_clk,
		 tx_busy => mac_tx_busy,
		 tx_byte_sent => mac_tx_byte_sent,
		 audio_sync => tdm_fs,
		 audio_ch0_in => SYNTHESIZED_WIRE_48,
		 audio_ch10_in => SYNTHESIZED_WIRE_49,
		 audio_ch11_in => SYNTHESIZED_WIRE_50,
		 audio_ch12_in => SYNTHESIZED_WIRE_51,
		 audio_ch13_in => SYNTHESIZED_WIRE_52,
		 audio_ch14_in => SYNTHESIZED_WIRE_53,
		 audio_ch15_in => SYNTHESIZED_WIRE_54,
		 audio_ch1_in => SYNTHESIZED_WIRE_55,
		 audio_ch2_in => SYNTHESIZED_WIRE_56,
		 audio_ch3_in => SYNTHESIZED_WIRE_57,
		 audio_ch4_in => SYNTHESIZED_WIRE_58,
		 audio_ch5_in => SYNTHESIZED_WIRE_59,
		 audio_ch6_in => SYNTHESIZED_WIRE_60,
		 audio_ch7_in => SYNTHESIZED_WIRE_61,
		 audio_ch8_in => SYNTHESIZED_WIRE_62,
		 audio_ch9_in => SYNTHESIZED_WIRE_63,
		 dst_ip_address => SYNTHESIZED_WIRE_64,
		 dst_mac_address => SYNTHESIZED_WIRE_65,
		 dst_udp_port => SYNTHESIZED_WIRE_66,
		 src_ip_address => SYNTHESIZED_WIRE_67,
		 src_mac_address => SYNTHESIZED_WIRE_87,
		 src_udp_port => SYNTHESIZED_WIRE_69);


b2v_inst5 : m8000adc_config
PORT MAP(clk => clk_16MHz,
		 i_start => online,
		 i_txbusy => SYNTHESIZED_WIRE_70,
		 o_start => SYNTHESIZED_WIRE_32,
		 o_done => SYNTHESIZED_WIRE_1,
		 o_address => SYNTHESIZED_WIRE_33,
		 o_data => SYNTHESIZED_WIRE_34,
		 o_map => SYNTHESIZED_WIRE_35);


b2v_inst51 : aes50_clk_ddr
PORT MAP(refclk => clk_50MHz,
		 reset => rst,
		 data => aes50_phy_clk_data,
		 dout(0) => aes50_phy_clk(0));


b2v_inst52 : audiomatrix_ram_write
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			NUM_INPUT_PORTS => 160,
			TDM_RECEIVER => 20
			)
PORT MAP(clk => clk_24_576MHz,
		 bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 input_data => SYNTHESIZED_WIRE_71,
		 o_ram_wr_en => SYNTHESIZED_WIRE_72,
		 o_ram_data => SYNTHESIZED_WIRE_73,
		 o_ram_write_addr => SYNTHESIZED_WIRE_75);


b2v_inst53 : audiomatrix_ram
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			RAM_DEPTH => 256
			)
PORT MAP(clk => clk_24_576MHz,
		 wr_en => SYNTHESIZED_WIRE_72,
		 i_data => SYNTHESIZED_WIRE_73,
		 read_addr => SYNTHESIZED_WIRE_74,
		 write_addr => SYNTHESIZED_WIRE_75,
		 o_data => SYNTHESIZED_WIRE_76);


b2v_inst54 : audiomatrix_ram_read
GENERIC MAP(ADDR_WIDTH => 8,
			DATA_WIDTH => 24,
			NUM_OUTPUT_PORTS => 160,
			TDM_SENDER => 20
			)
PORT MAP(clk => clk_24_576MHz,
		 bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ram_data => SYNTHESIZED_WIRE_76,
		 channel_idx_out => SYNTHESIZED_WIRE_10,
		 data_out => audio_output(479 DOWNTO 0));




b2v_inst7 : config_rxd
PORT MAP(clk => clk_24_576MHz,
		 cfg_wr_en => SYNTHESIZED_WIRE_84,
		 cfg_wr_addr => SYNTHESIZED_WIRE_85,
		 cfg_wr_data => SYNTHESIZED_WIRE_86);



b2v_inst8 : tdm_multichan_rx
GENERIC MAP(DATA_WIDTH => 24,
			TDM_RECEIVER => 20
			)
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 tdm_in => tdm_input,
		 data_out => SYNTHESIZED_WIRE_71);


b2v_inst9 : tdm_multichan_tx
GENERIC MAP(DATA_WIDTH => 24,
			TDM_RECEIVER => 20
			)
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 data_in => audio_output(479 DOWNTO 0),
		 tdm_out => tdm_output);


b2v_inst_pll : lattice_pll
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst);


b2v_inst_pll_phy : lattice_pll_phy
PORT MAP(CLKI => clk_16MHz,
		 RST => pripll_rst,
		 CLKOS => clk_50MHz);

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
D_CLK2 <= clk_24_576MHz;
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

aes50_phy_clk_data(1) <= '1';
aes50_phy_clk_data(0) <= '0';
aes50a_rmii_rxd_in(2) <= aes50a_rmii_rxd_1_in;
aes50a_rmii_rxd_in(1) <= aes50a_rmii_rxd_0_in;
aes50a_rmii_rxd_in(0) <= aes50a_rmii_crs_dv_in;
tdm_input(0) <= AD0_DATA1;
tdm_input(1) <= AD0_DATA0;
tdm_input(2) <= AD1_DATA1;
tdm_input(3) <= AD1_DATA0;
tdm_input(4) <= CARD_IN3;
tdm_input(5) <= CARD_IN2;
tdm_input(9) <= DSP_DOUT0;
tdm_input(10) <= DSP_DOUT1;
tdm_input(11) <= DSP_DOUT2;
tdm_input(12) <= DSP_DOUT3;
tdm_input(13) <= DSP_DOUTAUX;
tdm_input(8) <= AUX_AD;
tdm_input(6) <= CARD_IN1;
tdm_input(7) <= CARD_IN0;
END bdf_type;