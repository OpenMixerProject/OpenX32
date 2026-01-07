library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity zeros_const is
	port (
		const1 		: out std_logic_vector(23 downto 0);
		const2 		: out std_logic_vector(23 downto 0);
		const3 		: out std_logic_vector(23 downto 0);
		const4 		: out std_logic_vector(23 downto 0);
		const5 		: out std_logic_vector(23 downto 0);
		const6 		: out std_logic_vector(23 downto 0);
		const7 		: out std_logic_vector(23 downto 0);
		const8 		: out std_logic_vector(23 downto 0)
	);
end zeros_const;

architecture Behavioral of zeros_const is
begin
	const1 <= "000000000000000000000000";
	const2 <= "000000000000000000000000";
	const3 <= "000000000000000000000000";
	const4 <= "000000000000000000000000";
	const5 <= "000000000000000000000000";
	const6 <= "000000000000000000000000";
	const7 <= "000000000000000000000000";
	const8 <= "000000000000000000000000";
end Behavioral;