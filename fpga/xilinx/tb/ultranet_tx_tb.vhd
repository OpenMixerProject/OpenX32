library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ultranet_tx_tb is
end entity ultranet_tx_tb;

architecture test of ultranet_tx_tb is
	constant CLOCK_PERIOD : time := 40690 ps;	-- 24.576 MHz Clock for 192 kHz Stereo (= 4x 48kHz Stereo = 8 channel)
	--constant CLOCK_PERIOD : time := 162760 ps;	-- 6.144 MHz Clock for regular 48 kHz Stereo (= 2 channel)

	component ultranet_tx is
		port (
			bit_clock : in std_logic; -- 24.576MHz for 48K samplerate
			ch1 : in std_logic_vector(23 downto 0);
			ch2 : in std_logic_vector(23 downto 0);
			ch3 : in std_logic_vector(23 downto 0);
			ch4 : in std_logic_vector(23 downto 0);
			ch5 : in std_logic_vector(23 downto 0);
			ch6 : in std_logic_vector(23 downto 0);
			ch7 : in std_logic_vector(23 downto 0);
			ch8 : in std_logic_vector(23 downto 0);
			spdif_out : out std_logic
		);
	end component;
  
	signal tb_bitclk	: std_logic := '0';
	signal tb_ch1		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch2		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch3		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch4		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch5		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch6		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch7		: std_logic_vector(23 downto 0) := "111100000000000000000000";
	signal tb_ch8		: std_logic_vector(23 downto 0) := "111100000000000000000000";

	signal tb_spdif		: std_logic := '0';
begin
	-- Reset and clock
	tb_bitclk <= not tb_bitclk after CLOCK_PERIOD/2;

	-- declare the DUT module
	dut : ultranet_tx
    port map (
		bit_clock	=> tb_bitclk,
		ch1			=> tb_ch1,
		ch2			=> tb_ch2,
		ch3			=> tb_ch3,
		ch4			=> tb_ch4,
		ch5			=> tb_ch5,
		ch6			=> tb_ch6,
		ch7			=> tb_ch7,
		ch8			=> tb_ch8,
		spdif_out	=> tb_spdif
    );
end architecture test;