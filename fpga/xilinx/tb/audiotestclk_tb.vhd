-- Testbench for AudioTestClock
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiotestclk_tb is
end entity audiotestclk_tb;

architecture test of audiotestclk_tb is
	constant CLOCK_PERIOD : time := 8138 ps;	-- 122.88 MHz Clock

	component audiotestclk is
		port (
		clk		: in std_logic; -- 122.88 MHz for 48kHz
		o_sclk	: out std_logic; -- 12.288 MHz
		o_fs		: out std_logic -- 48 kHz
		);
	end component;
  
	signal tb_clk		: std_logic := '0';

	signal tb_sclk		: std_logic := '0';
	signal tb_fs		: std_logic := '0';
begin
	-- Reset and clock
	tb_clk <= not tb_clk after CLOCK_PERIOD/2;

	-- declare the DUT module
	dut : audiotestclk
    port map (
      clk		=> tb_clk,
	  o_sclk	=> tb_sclk,
	  o_fs		=> tb_fs
    );
end architecture test;