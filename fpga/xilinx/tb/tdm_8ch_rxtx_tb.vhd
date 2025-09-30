library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tdm_8ch_rxtx_tb is
end entity tdm_8ch_rxtx_tb;

architecture test of tdm_8ch_rxtx_tb is
	constant CLOCK_PERIOD : time := 8064 ps;	-- 124 MHz Clock

	component audiotestclk is
		port (
			clk		: in std_logic; -- 122.88 MHz
			o_sclk_a	: out std_logic; -- 24.576 MHz
			o_sclk_b	: out std_logic; -- 12.288 MHz
			o_fs		: out std_logic -- 48 kHz
		);
	end component;

	component tdm_8ch_tx is
		port (
			clk 	: in std_logic; -- mainclock
			bclk	: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
			fsync	: in std_logic; -- Frame sync (for X32 it is 384 kHz)
			ch1_in	: in std_logic_vector(23 downto 0);
			ch2_in	: in std_logic_vector(23 downto 0);
			ch3_in	: in std_logic_vector(23 downto 0);
			ch4_in	: in std_logic_vector(23 downto 0);
			ch5_in	: in std_logic_vector(23 downto 0);
			ch6_in	: in std_logic_vector(23 downto 0);
			ch7_in	: in std_logic_vector(23 downto 0);
			ch8_in	: in std_logic_vector(23 downto 0);

			sdata	: out std_logic -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
		);
	end component;
	
	component tdm_8ch_rx is
		port (
			clk 		: in std_logic; -- mainclock
			bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
			fsync		: in std_logic; -- Frame sync (for X32 it is 384 kHz)
			sdata		: in std_logic; -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
			
			ch1_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch2_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch3_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch4_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch5_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch6_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch7_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			ch8_out		: out std_logic_vector(23 downto 0); -- received audio-sample
			sync_out		: out std_logic -- new data received successfully
		  );
	end component;
  
	signal tb_clk		: std_logic := '0';
	signal tb_bclk_p16	: std_logic := '0';
	signal tb_bclk		: std_logic := '0';
	signal tb_fsync		: std_logic := '0';
	
	signal tb_ch1_tx	: std_logic_vector(23 downto 0) := "101100000000000000001010";
	signal tb_ch2_tx	: std_logic_vector(23 downto 0) := "100011000000000000001010";
	signal tb_ch3_tx	: std_logic_vector(23 downto 0) := "100000110000000000001010";
	signal tb_ch4_tx	: std_logic_vector(23 downto 0) := "100000001100000000001010";
	signal tb_ch5_tx	: std_logic_vector(23 downto 0) := "100000000011000000001010";
	signal tb_ch6_tx	: std_logic_vector(23 downto 0) := "100000000000110000001010";
	signal tb_ch7_tx	: std_logic_vector(23 downto 0) := "100000000000001100001010";
	signal tb_ch8_tx	: std_logic_vector(23 downto 0) := "100000000000000011001010";

	signal tb_ch1_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch2_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch3_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch4_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch5_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch6_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch7_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";
	signal tb_ch8_rx	: std_logic_vector(23 downto 0) := "000000000000000000000000";

	signal tb_sdata		: std_logic := '0';
begin
	-- Reset and clock
	tb_clk <= not tb_clk after CLOCK_PERIOD/2;

	-- declare the DUT modules
	
	dut_clk : audiotestclk
	port map (
		clk			=> tb_clk,
		o_sclk_a	=> tb_bclk_p16,
		o_sclk_b	=> tb_bclk,
		o_fs		=> tb_fsync
	);
	
	dut_tx : tdm_8ch_tx
    port map (
		clk		=> tb_clk,
		bclk	=> tb_bclk,
		fsync	=> tb_fsync,
		
		ch1_in	=> tb_ch1_tx,
		ch2_in	=> tb_ch2_tx,
		ch3_in	=> tb_ch3_tx,
		ch4_in	=> tb_ch4_tx,
		ch5_in	=> tb_ch5_tx,
		ch6_in	=> tb_ch6_tx,
		ch7_in	=> tb_ch7_tx,
		ch8_in	=> tb_ch8_tx,
		
		sdata	=> tb_sdata
    );

	dut_rx : tdm_8ch_rx
    port map (
		clk		=> tb_clk,
		bclk	=> tb_bclk,
		fsync	=> tb_fsync,
		sdata	=> tb_sdata,
		
		ch1_out	=> tb_ch1_rx,
		ch2_out	=> tb_ch2_rx,
		ch3_out	=> tb_ch3_rx,
		ch4_out	=> tb_ch4_rx,
		ch5_out	=> tb_ch5_rx,
		ch6_out	=> tb_ch6_rx,
		ch7_out	=> tb_ch7_rx,
		ch8_out	=> tb_ch8_rx
    );
end architecture test;