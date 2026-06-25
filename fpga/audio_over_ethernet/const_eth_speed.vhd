library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity const_eth_speed is
	port
	(
		eth_speed	: out std_logic_vector(1 downto 0)
	);
end entity;

architecture Behavioral of const_eth_speed is
begin
	eth_speed <= "01"; -- set fixed to 100MBit/s
end Behavioral;