library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity clkinverter is
	port (
		clk_i		: in std_logic;
		inv_i		: in std_logic;

		clk_o		: out std_logic
	);
end entity;

architecture behavioral of clkinverter is
begin
	clk_o <= clk_i when (inv_i = '0') else (not clk_i);
end behavioral;