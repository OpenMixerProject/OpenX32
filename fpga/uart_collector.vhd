library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity uart_collector is
	port (
		clk_in		: in std_logic; -- 16 MHz
		rst_in		: in std_logic;
		uart1_in	: in std_logic;
		uart2_in	: in std_logic;
		uart3_in	: in std_logic;
		uart4_in	: in std_logic;
		card_rdy : in std_logic;
		
		uart_out	: out std_logic
	);
end uart_collector;

architecture Behavioral of uart_collector is
	signal uart1_missing : std_logic;
	signal uart2_missing : std_logic;
	signal uart3_missing : std_logic;
	signal uart4_missing : std_logic;
begin
	process (clk_in)
	begin
		if rising_edge(clk_in) then
			if (rst_in = '1') then
				-- we are in reset -> get state of individual uarts
				-- if specific card is missing, the level is "0"
				uart1_missing <= not uart1_in;
				uart2_missing <= not uart2_in;
				uart3_missing <= not uart3_in;
			end if;
		end if;
	end process;

	uart4_missing <= not card_rdy;

	uart_out <= (uart1_missing or uart1_in) and (uart2_missing or uart2_in) and (uart3_missing or uart3_in) and (uart4_missing or uart4_in);
end Behavioral;