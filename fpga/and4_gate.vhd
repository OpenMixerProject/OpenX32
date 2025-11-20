library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity and4_gate is
	port (
		in1	: in std_logic;
		in2	: in std_logic;
		in3	: in std_logic;
		in4	: in std_logic;
		
		output	: out std_logic
	);
end and4_gate;

architecture Behavioral of and4_gate is
begin
	output <= (in1 and in2 and in3 and in4);
end Behavioral;