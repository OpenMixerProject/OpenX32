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
-- VERSION		"Version 24.1std.0 Build 1077 03/04/2025 SC Lite Edition"
-- CREATED		"Fri Oct 31 00:22:41 2025"

LIBRARY ieee;
USE ieee.std_logic_1164.all; 

LIBRARY work;

ENTITY main IS 
	PORT
	(
		fpgaclk :  IN  STD_LOGIC;
		PLL_OUT :  IN  STD_LOGIC;
		imx25_uart4_txd :  IN  STD_LOGIC;
		DA_RX :  IN  STD_LOGIC;
		AD0_RX :  IN  STD_LOGIC;
		AD1_RX :  IN  STD_LOGIC;
		CARD_RX :  IN  STD_LOGIC;
		imx25_uart3_txd :  IN  STD_LOGIC;
		AUX_AD :  IN  STD_LOGIC;
		PLL_nCS :  OUT  STD_LOGIC;
		PLL_CCLK :  OUT  STD_LOGIC;
		PLL_CDATA :  OUT  STD_LOGIC;
		imx25_uart4_rxd :  OUT  STD_LOGIC;
		DA_TX :  OUT  STD_LOGIC;
		AD0_TX :  OUT  STD_LOGIC;
		AD1_TX :  OUT  STD_LOGIC;
		CARD_TX :  OUT  STD_LOGIC;
		imx25_uart3_rxd :  OUT  STD_LOGIC;
		AUX_MCLK :  OUT  STD_LOGIC;
		AUX_BCLK :  OUT  STD_LOGIC;
		AUX_FSYNC :  OUT  STD_LOGIC;
		AUX_DA :  OUT  STD_LOGIC;
		AUX_CCLK :  OUT  STD_LOGIC;
		AUX_CDATA :  OUT  STD_LOGIC;
		AUX_nRST :  OUT  STD_LOGIC;
		AUX_AD_nCS :  OUT  STD_LOGIC;
		AUX_DA_nCS :  OUT  STD_LOGIC;
		DA_nRESET :  OUT  STD_LOGIC;
		AD_nRESET :  OUT  STD_LOGIC;
		CARD_nRESET :  OUT  STD_LOGIC
	);
END main;

ARCHITECTURE bdf_type OF main IS 

COMPONENT uart_rx
GENERIC (baud_rate : INTEGER;
			clk_rate : INTEGER
			);
	PORT(i_Clk : IN STD_LOGIC;
		 i_RX_Serial : IN STD_LOGIC;
		 o_RX_DV : OUT STD_LOGIC;
		 o_RX_Byte : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT reset
	PORT(clk : IN STD_LOGIC;
		 o_reset : OUT STD_LOGIC;
		 o_reset_inv : OUT STD_LOGIC;
		 o_startup : OUT STD_LOGIC
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

COMPONENT rs232_decoder
GENERIC (NUM_DSP_CHANNELS : INTEGER;
			NUM_OUTPUT_PORTS : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 RX_DataReady : IN STD_LOGIC;
		 RX_Data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 routing : OUT STD_LOGIC_VECTOR(895 DOWNTO 0)
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

COMPONENT or4_gate
	PORT(in1 : IN STD_LOGIC;
		 in2 : IN STD_LOGIC;
		 in3 : IN STD_LOGIC;
		 in4 : IN STD_LOGIC;
		 output : OUT STD_LOGIC
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

COMPONENT tdm_8ch_rx
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 sdata : IN STD_LOGIC;
		 sync_out : OUT STD_LOGIC;
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

COMPONENT tdm_8ch_tx
	PORT(bclk : IN STD_LOGIC;
		 fsync : IN STD_LOGIC;
		 ch1_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch2_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch3_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch4_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch5_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch6_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch7_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 ch8_in : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 sdata : OUT STD_LOGIC
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

COMPONENT audioclk
	PORT(i_clk : IN STD_LOGIC;
		 o_clk : OUT STD_LOGIC;
		 o_fs : OUT STD_LOGIC
	);
END COMPONENT;

COMPONENT rs232_encoder
GENERIC (clk_rate_hz : INTEGER;
			txd_rate_hz : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 TX_rdy : IN STD_LOGIC;
		 byte1 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte2 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte3 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte4 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 TX_send : OUT STD_LOGIC;
		 TX_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT rs232_const
	PORT(		 const1 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const2 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const3 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const4 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT uart_tx
GENERIC (baud_rate : INTEGER;
			clk_rate : INTEGER
			);
	PORT(i_Clk : IN STD_LOGIC;
		 i_TX_DV : IN STD_LOGIC;
		 i_TX_Byte : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 o_TX_Active : OUT STD_LOGIC;
		 o_TX_Serial : OUT STD_LOGIC;
		 o_TX_Done : OUT STD_LOGIC
	);
END COMPONENT;

SIGNAL	clk_12_288MHz :  STD_LOGIC;
SIGNAL	clk_16MHz :  STD_LOGIC;
SIGNAL	clk_24_576MHz :  STD_LOGIC;
SIGNAL	routing :  STD_LOGIC_VECTOR(895 DOWNTO 0);
SIGNAL	rst :  STD_LOGIC;
SIGNAL	rst_inv :  STD_LOGIC;
SIGNAL	start :  STD_LOGIC;
SIGNAL	tdm_fs :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_0 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_1 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_2 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_3 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_4 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_5 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_6 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_7 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_8 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_9 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_10 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_11 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_12 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_13 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_14 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_15 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_16 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_17 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_18 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_19 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_20 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_21 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_22 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_23 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_24 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_25 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_26 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_27 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_28 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_29 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_30 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_31 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_32 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_33 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_34 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_35 :  STD_LOGIC_VECTOR(7 DOWNTO 0);


BEGIN 
DA_TX <= imx25_uart3_txd;
AD0_TX <= imx25_uart3_txd;
AD1_TX <= imx25_uart3_txd;
CARD_TX <= imx25_uart3_txd;



b2v_inst : uart_rx
GENERIC MAP(baud_rate => 115200,
			clk_rate => 24576000
			)
PORT MAP(i_Clk => clk_24_576MHz,
		 i_RX_Serial => imx25_uart4_txd,
		 o_RX_DV => SYNTHESIZED_WIRE_1,
		 o_RX_Byte => SYNTHESIZED_WIRE_2);


b2v_inst0 : reset
PORT MAP(clk => clk_16MHz,
		 o_reset => rst,
		 o_reset_inv => rst_inv,
		 o_startup => start);


b2v_inst1 : cs2000cp_config
PORT MAP(clk => clk_16MHz,
		 i_start => rst,
		 i_txbusy => SYNTHESIZED_WIRE_0,
		 o_start => SYNTHESIZED_WIRE_9,
		 o_address => SYNTHESIZED_WIRE_10,
		 o_data => SYNTHESIZED_WIRE_11,
		 o_map => SYNTHESIZED_WIRE_12);


b2v_inst10 : rs232_decoder
GENERIC MAP(NUM_DSP_CHANNELS => 40,
			NUM_OUTPUT_PORTS => 112
			)
PORT MAP(clk => clk_24_576MHz,
		 RX_DataReady => SYNTHESIZED_WIRE_1,
		 RX_Data => SYNTHESIZED_WIRE_2);


b2v_inst11 : pcm1690dac_config
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_3,
		 i_txbusy => SYNTHESIZED_WIRE_4,
		 o_start => SYNTHESIZED_WIRE_13,
		 o_address => SYNTHESIZED_WIRE_14,
		 o_data => SYNTHESIZED_WIRE_15);


b2v_inst12 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_5,
		 in2 => SYNTHESIZED_WIRE_6,
		 output => AUX_CCLK);


b2v_inst13 : or2_gate
PORT MAP(in1 => SYNTHESIZED_WIRE_7,
		 in2 => SYNTHESIZED_WIRE_8,
		 output => AUX_CDATA);


b2v_inst18 : or4_gate
PORT MAP(in1 => DA_RX,
		 in2 => AD0_RX,
		 in3 => AD1_RX,
		 in4 => CARD_RX,
		 output => imx25_uart3_rxd);


b2v_inst2 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_9,
		 i_address => SYNTHESIZED_WIRE_10,
		 i_data => SYNTHESIZED_WIRE_11,
		 i_map => SYNTHESIZED_WIRE_12,
		 o_nCS => PLL_nCS,
		 o_cclk => PLL_CCLK,
		 o_cdata => PLL_CDATA,
		 o_busy => SYNTHESIZED_WIRE_0);


b2v_inst3 : spi_16bit_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_13,
		 i_address => SYNTHESIZED_WIRE_14,
		 i_data => SYNTHESIZED_WIRE_15,
		 o_nCS => AUX_DA_nCS,
		 o_cclk => SYNTHESIZED_WIRE_6,
		 o_cdata => SYNTHESIZED_WIRE_8,
		 o_busy => SYNTHESIZED_WIRE_4);


b2v_inst4 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_16,
		 i_address => SYNTHESIZED_WIRE_17,
		 i_data => SYNTHESIZED_WIRE_18,
		 i_map => SYNTHESIZED_WIRE_19,
		 o_nCS => AUX_AD_nCS,
		 o_cclk => SYNTHESIZED_WIRE_5,
		 o_cdata => SYNTHESIZED_WIRE_7,
		 o_busy => SYNTHESIZED_WIRE_28);


b2v_inst47 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AUX_AD,
		 ch1_out => SYNTHESIZED_WIRE_20,
		 ch2_out => SYNTHESIZED_WIRE_21,
		 ch3_out => SYNTHESIZED_WIRE_22,
		 ch4_out => SYNTHESIZED_WIRE_23,
		 ch5_out => SYNTHESIZED_WIRE_24,
		 ch6_out => SYNTHESIZED_WIRE_25,
		 ch7_out => SYNTHESIZED_WIRE_26,
		 ch8_out => SYNTHESIZED_WIRE_27);


b2v_inst48 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => SYNTHESIZED_WIRE_20,
		 ch2_in => SYNTHESIZED_WIRE_21,
		 ch3_in => SYNTHESIZED_WIRE_22,
		 ch4_in => SYNTHESIZED_WIRE_23,
		 ch5_in => SYNTHESIZED_WIRE_24,
		 ch6_in => SYNTHESIZED_WIRE_25,
		 ch7_in => SYNTHESIZED_WIRE_26,
		 ch8_in => SYNTHESIZED_WIRE_27,
		 sdata => AUX_DA);


b2v_inst5 : m8000adc_config
PORT MAP(clk => clk_16MHz,
		 i_start => start,
		 i_txbusy => SYNTHESIZED_WIRE_28,
		 o_start => SYNTHESIZED_WIRE_16,
		 o_done => SYNTHESIZED_WIRE_3,
		 o_address => SYNTHESIZED_WIRE_17,
		 o_data => SYNTHESIZED_WIRE_18,
		 o_map => SYNTHESIZED_WIRE_19);


b2v_inst6 : audioclk
PORT MAP(i_clk => clk_24_576MHz,
		 o_clk => clk_12_288MHz,
		 o_fs => tdm_fs);


b2v_inst7 : rs232_encoder
GENERIC MAP(clk_rate_hz => 24576000,
			txd_rate_hz => 1
			)
PORT MAP(clk => clk_24_576MHz,
		 TX_rdy => SYNTHESIZED_WIRE_29,
		 byte1 => SYNTHESIZED_WIRE_30,
		 byte2 => SYNTHESIZED_WIRE_31,
		 byte3 => SYNTHESIZED_WIRE_32,
		 byte4 => SYNTHESIZED_WIRE_33,
		 TX_send => SYNTHESIZED_WIRE_34,
		 TX_data => SYNTHESIZED_WIRE_35);


b2v_inst8 : rs232_const
PORT MAP(		 const1 => SYNTHESIZED_WIRE_30,
		 const2 => SYNTHESIZED_WIRE_31,
		 const3 => SYNTHESIZED_WIRE_32,
		 const4 => SYNTHESIZED_WIRE_33);


b2v_inst9 : uart_tx
GENERIC MAP(baud_rate => 115200,
			clk_rate => 24576000
			)
PORT MAP(i_Clk => clk_24_576MHz,
		 i_TX_DV => SYNTHESIZED_WIRE_34,
		 i_TX_Byte => SYNTHESIZED_WIRE_35,
		 o_TX_Serial => imx25_uart4_rxd,
		 o_TX_Done => SYNTHESIZED_WIRE_29);

clk_16MHz <= fpgaclk;
clk_24_576MHz <= PLL_OUT;
AUX_MCLK <= clk_12_288MHz;
AUX_BCLK <= clk_12_288MHz;
AUX_FSYNC <= tdm_fs;
AUX_nRST <= rst_inv;
DA_nRESET <= rst_inv;
AD_nRESET <= rst_inv;
CARD_nRESET <= rst_inv;

END bdf_type;