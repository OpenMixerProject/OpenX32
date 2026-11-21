library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_rst is
	port (
		clk50_i	: in std_logic;
		start_i	: in std_logic;
		
		rst_o		: out std_logic := '1'
	);
end entity;

architecture behavioral of aes50_rst is
	signal zstart	: std_logic := '0';
begin
	process (clk50_i)
	begin
		if rising_edge(clk50_i) then
			zstart <= start_i;
			
			if (start_i = '0') then
				-- system is not up
				rst_o <= '1';
			elsif (start_i = '1' and zstart = '0') then
				-- rising edge of start-signal -> system is up and running
				rst_o <= '0';
			end if;
		end if;
	end process;
end behavioral;