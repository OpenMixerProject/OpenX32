library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity rs232_const is
	port (
		const1 		: out std_logic_vector(7 downto 0);
		const2 		: out std_logic_vector(7 downto 0);
		const3 		: out std_logic_vector(7 downto 0);
		const4 		: out std_logic_vector(7 downto 0)
	);
end rs232_const;

architecture Behavioral of rs232_const is
begin
	const1 <= "01101111"; -- o
	const2 <= "01011000"; -- X
	const3 <= "00110011"; -- 3
	const4 <= "00110010"; -- 2
end Behavioral;