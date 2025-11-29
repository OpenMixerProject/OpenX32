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
-- CREATED		"Sat Nov 29 23:16:39 2025"

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
		DSP_DIN0 :  OUT  STD_LOGIC;
		DSP_DIN1 :  OUT  STD_LOGIC;
		DSP_DIN2 :  OUT  STD_LOGIC;
		DSP_DIN3 :  OUT  STD_LOGIC;
		D_CLK :  OUT  STD_LOGIC;
		D_FS :  OUT  STD_LOGIC;
		D_CLK2 :  OUT  STD_LOGIC;
		D_FS2 :  OUT  STD_LOGIC;
		DSP_DINAUX :  OUT  STD_LOGIC;
		P16_A_TXM :  OUT  STD_LOGIC;
		P16_A_TXP :  OUT  STD_LOGIC;
		P16_B_TXP :  OUT  STD_LOGIC;
		P16_B_TXM :  OUT  STD_LOGIC;
		DA_MCLK :  OUT  STD_LOGIC;
		DA_BCLK :  OUT  STD_LOGIC;
		DA_FSYNC :  OUT  STD_LOGIC;
		AUX_MCLK :  OUT  STD_LOGIC;
		AUX_BCLK :  OUT  STD_LOGIC;
		AUX_FSYNC :  OUT  STD_LOGIC;
		AUX_DA :  OUT  STD_LOGIC;
		AUX_CCLK :  OUT  STD_LOGIC;
		AUX_CDATA :  OUT  STD_LOGIC;
		AUX_nRST :  OUT  STD_LOGIC;
		DA_DATA1 :  OUT  STD_LOGIC;
		DA_DATA0 :  OUT  STD_LOGIC;
		CARD_OUT0 :  OUT  STD_LOGIC;
		CARD_OUT1 :  OUT  STD_LOGIC;
		CARD_OUT2 :  OUT  STD_LOGIC;
		CARD_OUT3 :  OUT  STD_LOGIC;
		AUX_AD_nCS :  OUT  STD_LOGIC;
		AUX_DA_nCS :  OUT  STD_LOGIC;
		DA_nRESET :  OUT  STD_LOGIC;
		AD_nRESET :  OUT  STD_LOGIC;
		CARD_nRESET :  OUT  STD_LOGIC;
		PLL_IN :  OUT  STD_LOGIC
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

COMPONENT and4_gate
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
GENERIC (DATA_WIDTH : INTEGER;
			NUM_INPUT_PORTS : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 sync_in : IN STD_LOGIC;
		 input_data : IN STD_LOGIC_VECTOR(2687 DOWNTO 0);
		 o_ram_wr_en : OUT STD_LOGIC;
		 o_write_done : OUT STD_LOGIC;
		 o_ram_data : OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
		 o_ram_write_addr : OUT STD_LOGIC_VECTOR(6 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audiomatrix_ram
GENERIC (DATA_WIDTH : INTEGER;
			NUM_INPUT_PORTS : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 wr_en : IN STD_LOGIC;
		 i_data : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 read_addr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
		 write_addr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
		 o_data : OUT STD_LOGIC_VECTOR(23 DOWNTO 0)
	);
END COMPONENT;

COMPONENT audiomatrix_ram_read
GENERIC (DATA_WIDTH : INTEGER;
			NUM_OUTPUT_PORTS : INTEGER
			);
	PORT(clk : IN STD_LOGIC;
		 sync_in : IN STD_LOGIC;
		 i_ram_data : IN STD_LOGIC_VECTOR(23 DOWNTO 0);
		 select_lines : IN STD_LOGIC_VECTOR(895 DOWNTO 0);
		 o_ram_read_addr : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		 output_data : OUT STD_LOGIC_VECTOR(2687 DOWNTO 0)
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
		 byte5 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte6 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte7 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte8 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 byte9 : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 TX_send : OUT STD_LOGIC;
		 TX_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

COMPONENT rs232_const
	PORT(		 const1 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const2 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const3 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const4 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const5 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const6 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const7 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const8 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		 const9 : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
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

SIGNAL	audio_input :  STD_LOGIC_VECTOR(2687 DOWNTO 0);
SIGNAL	audio_output :  STD_LOGIC_VECTOR(2687 DOWNTO 0);
SIGNAL	audiosync :  STD_LOGIC;
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
SIGNAL	SYNTHESIZED_WIRE_20 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_21 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_22 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_23 :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_24 :  STD_LOGIC_VECTOR(6 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_25 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_26 :  STD_LOGIC_VECTOR(23 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_27 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_28 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_29 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_30 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_31 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_32 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_33 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_34 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_35 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_36 :  STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL	SYNTHESIZED_WIRE_37 :  STD_LOGIC;
SIGNAL	SYNTHESIZED_WIRE_38 :  STD_LOGIC_VECTOR(7 DOWNTO 0);


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
		 RX_Data => SYNTHESIZED_WIRE_2,
		 routing => routing);


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


b2v_inst14 : and4_gate
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


b2v_inst21 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AD0_DATA1,
		 ch1_out => audio_input(23 DOWNTO 0),
		 ch2_out => audio_input(47 DOWNTO 24),
		 ch3_out => audio_input(71 DOWNTO 48),
		 ch4_out => audio_input(95 DOWNTO 72),
		 ch5_out => audio_input(119 DOWNTO 96),
		 ch6_out => audio_input(143 DOWNTO 120),
		 ch7_out => audio_input(167 DOWNTO 144),
		 ch8_out => audio_input(191 DOWNTO 168));


b2v_inst22 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AD0_DATA0,
		 ch1_out => audio_input(407 DOWNTO 384),
		 ch2_out => audio_input(431 DOWNTO 408),
		 ch3_out => audio_input(455 DOWNTO 432),
		 ch4_out => audio_input(479 DOWNTO 456),
		 ch5_out => audio_input(503 DOWNTO 480),
		 ch6_out => audio_input(527 DOWNTO 504),
		 ch7_out => audio_input(551 DOWNTO 528),
		 ch8_out => audio_input(575 DOWNTO 552));


b2v_inst23 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AD1_DATA1,
		 ch1_out => audio_input(215 DOWNTO 192),
		 ch2_out => audio_input(239 DOWNTO 216),
		 ch3_out => audio_input(263 DOWNTO 240),
		 ch4_out => audio_input(287 DOWNTO 264),
		 ch5_out => audio_input(311 DOWNTO 288),
		 ch6_out => audio_input(335 DOWNTO 312),
		 ch7_out => audio_input(359 DOWNTO 336),
		 ch8_out => audio_input(383 DOWNTO 360));


b2v_inst24 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AD1_DATA0,
		 ch1_out => audio_input(599 DOWNTO 576),
		 ch2_out => audio_input(623 DOWNTO 600),
		 ch3_out => audio_input(647 DOWNTO 624),
		 ch4_out => audio_input(671 DOWNTO 648),
		 ch5_out => audio_input(695 DOWNTO 672),
		 ch6_out => audio_input(719 DOWNTO 696),
		 ch7_out => audio_input(743 DOWNTO 720),
		 ch8_out => audio_input(767 DOWNTO 744));


b2v_inst25 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => CARD_IN3,
		 ch1_out => audio_input(791 DOWNTO 768),
		 ch2_out => audio_input(815 DOWNTO 792),
		 ch3_out => audio_input(839 DOWNTO 816),
		 ch4_out => audio_input(863 DOWNTO 840),
		 ch5_out => audio_input(887 DOWNTO 864),
		 ch6_out => audio_input(911 DOWNTO 888),
		 ch7_out => audio_input(935 DOWNTO 912),
		 ch8_out => audio_input(959 DOWNTO 936));


b2v_inst26 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => CARD_IN1,
		 ch1_out => audio_input(1175 DOWNTO 1152),
		 ch2_out => audio_input(1199 DOWNTO 1176),
		 ch3_out => audio_input(1223 DOWNTO 1200),
		 ch4_out => audio_input(1247 DOWNTO 1224),
		 ch5_out => audio_input(1271 DOWNTO 1248),
		 ch6_out => audio_input(1295 DOWNTO 1272),
		 ch7_out => audio_input(1319 DOWNTO 1296),
		 ch8_out => audio_input(1343 DOWNTO 1320));


b2v_inst27 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => CARD_IN2,
		 ch1_out => audio_input(983 DOWNTO 960),
		 ch2_out => audio_input(1007 DOWNTO 984),
		 ch3_out => audio_input(1031 DOWNTO 1008),
		 ch4_out => audio_input(1055 DOWNTO 1032),
		 ch5_out => audio_input(1079 DOWNTO 1056),
		 ch6_out => audio_input(1103 DOWNTO 1080),
		 ch7_out => audio_input(1127 DOWNTO 1104),
		 ch8_out => audio_input(1151 DOWNTO 1128));


b2v_inst28 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => CARD_IN0,
		 ch1_out => audio_input(1367 DOWNTO 1344),
		 ch2_out => audio_input(1391 DOWNTO 1368),
		 ch3_out => audio_input(1415 DOWNTO 1392),
		 ch4_out => audio_input(1439 DOWNTO 1416),
		 ch5_out => audio_input(1463 DOWNTO 1440),
		 ch6_out => audio_input(1487 DOWNTO 1464),
		 ch7_out => audio_input(1511 DOWNTO 1488),
		 ch8_out => audio_input(1535 DOWNTO 1512));


b2v_inst29 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => DSP_DOUT0,
		 ch1_out => audio_input(1751 DOWNTO 1728),
		 ch2_out => audio_input(1775 DOWNTO 1752),
		 ch3_out => audio_input(1799 DOWNTO 1776),
		 ch4_out => audio_input(1823 DOWNTO 1800),
		 ch5_out => audio_input(1847 DOWNTO 1824),
		 ch6_out => audio_input(1871 DOWNTO 1848),
		 ch7_out => audio_input(1895 DOWNTO 1872),
		 ch8_out => audio_input(1919 DOWNTO 1896));


b2v_inst3 : spi_16bit_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_13,
		 i_address => SYNTHESIZED_WIRE_14,
		 i_data => SYNTHESIZED_WIRE_15,
		 o_nCS => AUX_DA_nCS,
		 o_cclk => SYNTHESIZED_WIRE_6,
		 o_cdata => SYNTHESIZED_WIRE_8,
		 o_busy => SYNTHESIZED_WIRE_4);


b2v_inst30 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => DSP_DOUT1,
		 ch1_out => audio_input(1943 DOWNTO 1920),
		 ch2_out => audio_input(1967 DOWNTO 1944),
		 ch3_out => audio_input(1991 DOWNTO 1968),
		 ch4_out => audio_input(2015 DOWNTO 1992),
		 ch5_out => audio_input(2039 DOWNTO 2016),
		 ch6_out => audio_input(2063 DOWNTO 2040),
		 ch7_out => audio_input(2087 DOWNTO 2064),
		 ch8_out => audio_input(2111 DOWNTO 2088));


b2v_inst31 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => DSP_DOUT2,
		 ch1_out => audio_input(2135 DOWNTO 2112),
		 ch2_out => audio_input(2159 DOWNTO 2136),
		 ch3_out => audio_input(2183 DOWNTO 2160),
		 ch4_out => audio_input(2207 DOWNTO 2184),
		 ch5_out => audio_input(2231 DOWNTO 2208),
		 ch6_out => audio_input(2255 DOWNTO 2232),
		 ch7_out => audio_input(2279 DOWNTO 2256),
		 ch8_out => audio_input(2303 DOWNTO 2280));


b2v_inst32 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => DSP_DOUT3,
		 ch1_out => audio_input(2327 DOWNTO 2304),
		 ch2_out => audio_input(2351 DOWNTO 2328),
		 ch3_out => audio_input(2375 DOWNTO 2352),
		 ch4_out => audio_input(2399 DOWNTO 2376),
		 ch5_out => audio_input(2423 DOWNTO 2400),
		 ch6_out => audio_input(2447 DOWNTO 2424),
		 ch7_out => audio_input(2471 DOWNTO 2448),
		 ch8_out => audio_input(2495 DOWNTO 2472));


b2v_inst33 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(1751 DOWNTO 1728),
		 ch2_in => audio_output(1775 DOWNTO 1752),
		 ch3_in => audio_output(1799 DOWNTO 1776),
		 ch4_in => audio_output(1823 DOWNTO 1800),
		 ch5_in => audio_output(1847 DOWNTO 1824),
		 ch6_in => audio_output(1871 DOWNTO 1848),
		 ch7_in => audio_output(1895 DOWNTO 1872),
		 ch8_in => audio_output(1919 DOWNTO 1896),
		 sdata => DSP_DIN0);


b2v_inst34 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(1943 DOWNTO 1920),
		 ch2_in => audio_output(1967 DOWNTO 1944),
		 ch3_in => audio_output(1991 DOWNTO 1968),
		 ch4_in => audio_output(2015 DOWNTO 1992),
		 ch5_in => audio_output(2039 DOWNTO 2016),
		 ch6_in => audio_output(2063 DOWNTO 2040),
		 ch7_in => audio_output(2087 DOWNTO 2064),
		 ch8_in => audio_output(2111 DOWNTO 2088),
		 sdata => DSP_DIN1);


b2v_inst35 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(2135 DOWNTO 2112),
		 ch2_in => audio_output(2159 DOWNTO 2136),
		 ch3_in => audio_output(2183 DOWNTO 2160),
		 ch4_in => audio_output(2207 DOWNTO 2184),
		 ch5_in => audio_output(2231 DOWNTO 2208),
		 ch6_in => audio_output(2255 DOWNTO 2232),
		 ch7_in => audio_output(2279 DOWNTO 2256),
		 ch8_in => audio_output(2303 DOWNTO 2280),
		 sdata => DSP_DIN2);


b2v_inst36 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(2327 DOWNTO 2304),
		 ch2_in => audio_output(2351 DOWNTO 2328),
		 ch3_in => audio_output(2375 DOWNTO 2352),
		 ch4_in => audio_output(2399 DOWNTO 2376),
		 ch5_in => audio_output(2423 DOWNTO 2400),
		 ch6_in => audio_output(2447 DOWNTO 2424),
		 ch7_in => audio_output(2471 DOWNTO 2448),
		 ch8_in => audio_output(2495 DOWNTO 2472),
		 sdata => DSP_DIN3);


b2v_inst37 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => DSP_DOUTAUX,
		 ch1_out => audio_input(2519 DOWNTO 2496),
		 ch2_out => audio_input(2543 DOWNTO 2520),
		 ch3_out => audio_input(2567 DOWNTO 2544),
		 ch4_out => audio_input(2591 DOWNTO 2568),
		 ch5_out => audio_input(2615 DOWNTO 2592),
		 ch6_out => audio_input(2639 DOWNTO 2616),
		 ch7_out => audio_input(2663 DOWNTO 2640),
		 ch8_out => audio_input(2687 DOWNTO 2664));


b2v_inst38 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(2519 DOWNTO 2496),
		 ch2_in => audio_output(2543 DOWNTO 2520),
		 ch3_in => audio_output(2567 DOWNTO 2544),
		 ch4_in => audio_output(2591 DOWNTO 2568),
		 ch5_in => audio_output(2615 DOWNTO 2592),
		 ch6_in => audio_output(2639 DOWNTO 2616),
		 ch7_in => audio_output(2663 DOWNTO 2640),
		 ch8_in => audio_output(2687 DOWNTO 2664),
		 sdata => DSP_DINAUX);


b2v_inst39 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => audio_output(407 DOWNTO 384),
		 ch2 => audio_output(431 DOWNTO 408),
		 ch3 => audio_output(455 DOWNTO 432),
		 ch4 => audio_output(479 DOWNTO 456),
		 ch5 => audio_output(503 DOWNTO 480),
		 ch6 => audio_output(527 DOWNTO 504),
		 ch7 => audio_output(551 DOWNTO 528),
		 ch8 => audio_output(575 DOWNTO 552),
		 ultranet_out_p => P16_A_TXP,
		 ultranet_out_m => P16_A_TXM);


b2v_inst4 : spi_tx
PORT MAP(clk => clk_16MHz,
		 i_start => SYNTHESIZED_WIRE_16,
		 i_address => SYNTHESIZED_WIRE_17,
		 i_data => SYNTHESIZED_WIRE_18,
		 i_map => SYNTHESIZED_WIRE_19,
		 o_nCS => AUX_AD_nCS,
		 o_cclk => SYNTHESIZED_WIRE_5,
		 o_cdata => SYNTHESIZED_WIRE_7,
		 o_busy => SYNTHESIZED_WIRE_20);


b2v_inst40 : ultranet_tx
GENERIC MAP(AES3_PREAMBLE_X => "10010011",
			AES3_PREAMBLE_Y => "10010110",
			AES3_PREAMBLE_Z => "10011100",
			FRAME_COUNTER_RESET => "101111111"
			)
PORT MAP(bit_clock => clk_24_576MHz,
		 ch1 => audio_output(599 DOWNTO 576),
		 ch2 => audio_output(623 DOWNTO 600),
		 ch3 => audio_output(647 DOWNTO 624),
		 ch4 => audio_output(671 DOWNTO 648),
		 ch5 => audio_output(695 DOWNTO 672),
		 ch6 => audio_output(719 DOWNTO 696),
		 ch7 => audio_output(743 DOWNTO 720),
		 ch8 => audio_output(767 DOWNTO 744),
		 ultranet_out_p => P16_B_TXP,
		 ultranet_out_m => P16_B_TXM);


b2v_inst41 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(791 DOWNTO 768),
		 ch2_in => audio_output(815 DOWNTO 792),
		 ch3_in => audio_output(839 DOWNTO 816),
		 ch4_in => audio_output(863 DOWNTO 840),
		 ch5_in => audio_output(887 DOWNTO 864),
		 ch6_in => audio_output(911 DOWNTO 888),
		 ch7_in => audio_output(935 DOWNTO 912),
		 ch8_in => audio_output(959 DOWNTO 936),
		 sdata => CARD_OUT0);


b2v_inst42 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(1175 DOWNTO 1152),
		 ch2_in => audio_output(1199 DOWNTO 1176),
		 ch3_in => audio_output(1223 DOWNTO 1200),
		 ch4_in => audio_output(1247 DOWNTO 1224),
		 ch5_in => audio_output(1271 DOWNTO 1248),
		 ch6_in => audio_output(1295 DOWNTO 1272),
		 ch7_in => audio_output(1319 DOWNTO 1296),
		 ch8_in => audio_output(1343 DOWNTO 1320),
		 sdata => CARD_OUT2);


b2v_inst43 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(983 DOWNTO 960),
		 ch2_in => audio_output(1007 DOWNTO 984),
		 ch3_in => audio_output(1031 DOWNTO 1008),
		 ch4_in => audio_output(1055 DOWNTO 1032),
		 ch5_in => audio_output(1079 DOWNTO 1056),
		 ch6_in => audio_output(1103 DOWNTO 1080),
		 ch7_in => audio_output(1127 DOWNTO 1104),
		 ch8_in => audio_output(1151 DOWNTO 1128),
		 sdata => CARD_OUT1);


b2v_inst44 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(1367 DOWNTO 1344),
		 ch2_in => audio_output(1391 DOWNTO 1368),
		 ch3_in => audio_output(1415 DOWNTO 1392),
		 ch4_in => audio_output(1439 DOWNTO 1416),
		 ch5_in => audio_output(1463 DOWNTO 1440),
		 ch6_in => audio_output(1487 DOWNTO 1464),
		 ch7_in => audio_output(1511 DOWNTO 1488),
		 ch8_in => audio_output(1535 DOWNTO 1512),
		 sdata => CARD_OUT3);


b2v_inst45 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(23 DOWNTO 0),
		 ch2_in => audio_output(47 DOWNTO 24),
		 ch3_in => audio_output(71 DOWNTO 48),
		 ch4_in => audio_output(95 DOWNTO 72),
		 ch5_in => audio_output(119 DOWNTO 96),
		 ch6_in => audio_output(143 DOWNTO 120),
		 ch7_in => audio_output(167 DOWNTO 144),
		 ch8_in => audio_output(191 DOWNTO 168),
		 sdata => DA_DATA1);


b2v_inst46 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(215 DOWNTO 192),
		 ch2_in => audio_output(239 DOWNTO 216),
		 ch3_in => audio_output(263 DOWNTO 240),
		 ch4_in => audio_output(287 DOWNTO 264),
		 ch5_in => audio_output(311 DOWNTO 288),
		 ch6_in => audio_output(335 DOWNTO 312),
		 ch7_in => audio_output(359 DOWNTO 336),
		 ch8_in => audio_output(383 DOWNTO 360),
		 sdata => DA_DATA0);


b2v_inst47 : tdm_8ch_rx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 sdata => AUX_AD,
		 sync_out => audiosync,
		 ch1_out => audio_input(1559 DOWNTO 1536),
		 ch2_out => audio_input(1583 DOWNTO 1560),
		 ch3_out => audio_input(1607 DOWNTO 1584),
		 ch4_out => audio_input(1631 DOWNTO 1608),
		 ch5_out => audio_input(1655 DOWNTO 1632),
		 ch6_out => audio_input(1679 DOWNTO 1656),
		 ch7_out => audio_input(1703 DOWNTO 1680),
		 ch8_out => audio_input(1727 DOWNTO 1704));


b2v_inst48 : tdm_8ch_tx
PORT MAP(bclk => clk_12_288MHz,
		 fsync => tdm_fs,
		 ch1_in => audio_output(1559 DOWNTO 1536),
		 ch2_in => audio_output(1583 DOWNTO 1560),
		 ch3_in => audio_output(1607 DOWNTO 1584),
		 ch4_in => audio_output(1631 DOWNTO 1608),
		 ch5_in => audio_output(1655 DOWNTO 1632),
		 ch6_in => audio_output(1679 DOWNTO 1656),
		 ch7_in => audio_output(1703 DOWNTO 1680),
		 ch8_in => audio_output(1727 DOWNTO 1704),
		 sdata => AUX_DA);


b2v_inst5 : m8000adc_config
PORT MAP(clk => clk_16MHz,
		 i_start => start,
		 i_txbusy => SYNTHESIZED_WIRE_20,
		 o_start => SYNTHESIZED_WIRE_16,
		 o_done => SYNTHESIZED_WIRE_3,
		 o_address => SYNTHESIZED_WIRE_17,
		 o_data => SYNTHESIZED_WIRE_18,
		 o_map => SYNTHESIZED_WIRE_19);


b2v_inst52 : audiomatrix_ram_write
GENERIC MAP(DATA_WIDTH => 24,
			NUM_INPUT_PORTS => 112
			)
PORT MAP(clk => clk_24_576MHz,
		 sync_in => audiosync,
		 input_data => audio_input,
		 o_ram_wr_en => SYNTHESIZED_WIRE_21,
		 o_write_done => SYNTHESIZED_WIRE_25,
		 o_ram_data => SYNTHESIZED_WIRE_22,
		 o_ram_write_addr => SYNTHESIZED_WIRE_24);


b2v_inst53 : audiomatrix_ram
GENERIC MAP(DATA_WIDTH => 24,
			NUM_INPUT_PORTS => 112
			)
PORT MAP(clk => clk_24_576MHz,
		 wr_en => SYNTHESIZED_WIRE_21,
		 i_data => SYNTHESIZED_WIRE_22,
		 read_addr => SYNTHESIZED_WIRE_23,
		 write_addr => SYNTHESIZED_WIRE_24,
		 o_data => SYNTHESIZED_WIRE_26);


b2v_inst54 : audiomatrix_ram_read
GENERIC MAP(DATA_WIDTH => 24,
			NUM_OUTPUT_PORTS => 112
			)
PORT MAP(clk => clk_24_576MHz,
		 sync_in => SYNTHESIZED_WIRE_25,
		 i_ram_data => SYNTHESIZED_WIRE_26,
		 select_lines => routing,
		 o_ram_read_addr => SYNTHESIZED_WIRE_23,
		 output_data => audio_output);


b2v_inst6 : audioclk
PORT MAP(i_clk => clk_24_576MHz,
		 o_clk => clk_12_288MHz,
		 o_fs => tdm_fs);


b2v_inst7 : rs232_encoder
GENERIC MAP(clk_rate_hz => 24576000,
			txd_rate_hz => 1
			)
PORT MAP(clk => clk_24_576MHz,
		 TX_rdy => SYNTHESIZED_WIRE_27,
		 byte1 => SYNTHESIZED_WIRE_28,
		 byte2 => SYNTHESIZED_WIRE_29,
		 byte3 => SYNTHESIZED_WIRE_30,
		 byte4 => SYNTHESIZED_WIRE_31,
		 byte5 => SYNTHESIZED_WIRE_32,
		 byte6 => SYNTHESIZED_WIRE_33,
		 byte7 => SYNTHESIZED_WIRE_34,
		 byte8 => SYNTHESIZED_WIRE_35,
		 byte9 => SYNTHESIZED_WIRE_36,
		 TX_send => SYNTHESIZED_WIRE_37,
		 TX_data => SYNTHESIZED_WIRE_38);


b2v_inst8 : rs232_const
PORT MAP(		 const1 => SYNTHESIZED_WIRE_28,
		 const2 => SYNTHESIZED_WIRE_29,
		 const3 => SYNTHESIZED_WIRE_30,
		 const4 => SYNTHESIZED_WIRE_31,
		 const5 => SYNTHESIZED_WIRE_32,
		 const6 => SYNTHESIZED_WIRE_33,
		 const7 => SYNTHESIZED_WIRE_34,
		 const8 => SYNTHESIZED_WIRE_35,
		 const9 => SYNTHESIZED_WIRE_36);


b2v_inst9 : uart_tx
GENERIC MAP(baud_rate => 115200,
			clk_rate => 24576000
			)
PORT MAP(i_Clk => clk_24_576MHz,
		 i_TX_DV => SYNTHESIZED_WIRE_37,
		 i_TX_Byte => SYNTHESIZED_WIRE_38,
		 o_TX_Serial => imx25_uart4_rxd,
		 o_TX_Done => SYNTHESIZED_WIRE_27);

clk_16MHz <= fpgaclk;
clk_24_576MHz <= PLL_OUT;
AD0_MCLK <= clk_12_288MHz;
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
AUX_nRST <= rst_inv;
DA_nRESET <= rst_inv;
AD_nRESET <= rst_inv;
CARD_nRESET <= rst_inv;
PLL_IN <= clk_16MHz;

END bdf_type;