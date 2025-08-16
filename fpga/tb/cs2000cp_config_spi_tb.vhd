-- Testbench for CS2000CP Config
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cs2000cp_config_spi_tb is
end entity cs2000cp_config_spi_tb;

architecture test of cs2000cp_config_spi_tb is
	constant CLOCK_PERIOD : time := 62500 ps;	-- 16 MHz Clock

	component cs2000cp_config is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_start		: in std_logic;
		i_txbusy		: in std_logic;
		
		o_address	: out std_logic_vector(7 downto 0);
		o_map			: out std_logic_vector(7 downto 0);
		o_data		: out std_logic_vector(7 downto 0);
		o_start		: out std_logic
	);
	end component;
  
	component spi_tx is
		port (
			clk			: in std_logic; -- expecting 16 MHz
			i_address	: in std_logic_vector(7 downto 0);
			i_map		: in std_logic_vector(7 downto 0);
			i_data		: in std_logic_vector(7 downto 0);
			i_start		: in std_logic;
			
			o_nCS		: out std_logic;
			o_cclk		: out std_logic;
			o_cdata		: out std_logic;
			o_busy		: out std_logic
		);
	end component;

	signal tb_clk		: std_logic := '0';

	-- signals for CS2000CP
	signal tb_start		: std_logic := '0';
	signal tb_address	: std_logic_vector(7 downto 0);
	signal tb_map		: std_logic_vector(7 downto 0);
	signal tb_data		: std_logic_vector(7 downto 0);
	signal tb_ostart	: std_logic := '0';

	-- signals for SPI-TX
	signal tb_nCS		: std_logic := '1';
	signal tb_cclk		: std_logic := '0';
	signal tb_cdata		: std_logic := '0';
	signal tb_busy		: std_logic := '0';
begin
	-- Reset and clock
	tb_clk <= not tb_clk after CLOCK_PERIOD/2;
	tb_start <= '0', '1' after 2*CLOCK_PERIOD, '0' after 4*CLOCK_PERIOD;

	-- declare the DUT module
	dut1 : cs2000cp_config
    port map (
      clk		=> tb_clk,
	  i_start	=> tb_start,
	  i_txbusy	=> tb_busy,
	  
	  o_address	=> tb_address,
	  o_map		=> tb_map,
	  o_data	=> tb_data,
	  o_start	=> tb_ostart
    );

	dut2 : spi_tx
    port map (
      clk		=> tb_clk,
      i_address	=> tb_address,
      i_map		=> tb_map,
      i_data	=> tb_data,
      i_start	=> tb_ostart,
	  
      o_nCS		=> tb_nCS,
      o_cclk	=> tb_cclk,
      o_cdata	=> tb_cdata,
      o_busy	=> tb_busy
    );

end architecture test;