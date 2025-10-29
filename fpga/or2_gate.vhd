library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity or2_gate is
	port (
		in1	: in std_logic;
		in2	: in std_logic;
		
		output	: out std_logic
	);
end or2_gate;

architecture Behavioral of or2_gate is
begin
	output <= in1 or in2;
end Behavioral;