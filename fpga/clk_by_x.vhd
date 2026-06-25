LIBRARY IEEE;    
USE IEEE.STD_LOGIC_1164.ALL;    

entity clk_by_x is
	generic(
		divider	: natural range 2 to 20000 := 16000
	);
    port (
        clk_in : in std_logic;
        clk_out : out std_logic
    );
end clk_by_x;

architecture Behavioral of clk_by_x is
    signal count : integer := 0;
    signal b : std_logic :='0';
begin
    process(clk_in)     
    begin
        if(rising_edge(clk_in)) then
            count <= count + 1;
            if(count = divider - 1) then
                b <= not b;
                count <= 0;
            end if;
        end if;
    end process;

    clk_out<=b;
end;
