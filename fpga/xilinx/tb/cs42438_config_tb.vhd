-- Testbench for CS42438 Config
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cs42438_config_tb is
end entity cs42438_config_tb;

architecture test of cs42438_config_tb is
	constant CLOCK_PERIOD : time := 62500 ps;	-- 16 MHz Clock

	component cs42438_config is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_start		: in std_logic;
		i_txbusy	: in std_logic;
		
		o_nRST		: out std_logic;
		o_address	: out std_logic_vector(7 downto 0);
		o_map		: out std_logic_vector(7 downto 0);
		o_data		: out std_logic_vector(7 downto 0);
		o_start		: out std_logic
	);
	end component;
  
	signal tb_clk		: std_logic := '0';

	signal tb_start		: std_logic := '0';
	signal tb_txbusy	: std_logic := '0';
	signal tb_nRST		: std_logic := '0';
	signal tb_address	: std_logic_vector(7 downto 0);
	signal tb_map		: std_logic_vector(7 downto 0);
	signal tb_data		: std_logic_vector(7 downto 0);
	signal tb_ostart	: std_logic := '0';
begin
	-- Reset and clock
	tb_clk <= not tb_clk after CLOCK_PERIOD/2;
	tb_start <= '0', '1' after 2*CLOCK_PERIOD, '0' after 4*CLOCK_PERIOD;
	--tb_txbusy <= '0', '1' after 366 ns, '0' after 1000 ns;

	-- declare the DUT module
	dut : cs42438_config
    port map (
      clk		=> tb_clk,
	  i_start	=> tb_start,
	  i_txbusy	=> tb_txbusy,
	  
	  o_nRST	=> tb_nRST,
	  o_address	=> tb_address,
	  o_map		=> tb_map,
	  o_data	=> tb_data,
	  o_start	=> tb_ostart
    );
end architecture test;