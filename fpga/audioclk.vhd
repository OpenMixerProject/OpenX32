-- VHDL File to control Cirrus Logic CS42438 IC
-- v0.0.1, 24.07.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audioclk is
	port (
		i_clk		: in std_logic; -- 24.576 MHz
		o_clk		: out std_logic; -- 12.288 MHz
		o_fs		: out std_logic -- 48 kHz
	);
end entity;

architecture behavioral of audioclk is
	signal count_fs		: natural range 0 to 256 := 1;
	signal clk				: std_logic := '0';
	signal fs				: std_logic := '0';
begin
	process (i_clk)
	begin
		if rising_edge(i_clk) then
			if (clk = '0') then
				-- rising edge of clk
				clk <= '1';
			else
				-- falling edge of clk
				clk <= '0';

				if (count_fs = (12288000/(2*48000))) then -- divide sclk by 256 (12.288 MHz -> 48 kHz)
					fs <= not fs;
					count_fs <= 1;
				else
					count_fs <= count_fs + 1;
				end if;
			end if;
		end if;
	end process;
	
	-- output the signals
	o_clk <= clk;
	o_fs <= fs;
end behavioral;