library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity or4_gate is
	port (
		in1	: in std_logic;
		in2	: in std_logic;
		in3	: in std_logic;
		in4	: in std_logic;
		
		output	: out std_logic
	);
end or4_gate;

architecture Behavioral of or4_gate is
begin
	output <= in1 or in2 or in3 or in4;
end Behavioral;