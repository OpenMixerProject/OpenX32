library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity bclk_2_fsync is
	port (
		bclk_i	: in std_logic; -- 12.288 MHz
		
		fsync_o	: out std_logic -- 48 kHz
	);
end entity;

architecture behavioral of bclk_2_fsync is
	signal count_fs		: natural range 0 to 256 := 1;
	signal fs				: std_logic := '0';
begin
	process (bclk_i)
	begin
		if falling_edge(bclk_i) then
			if (count_fs = (12288000/(2*48000))) then -- divide sclk by 256 (12.288 MHz -> 48 kHz)
				fs <= not fs;
				count_fs <= 1;
			else
				count_fs <= count_fs + 1;
			end if;
		end if;
	end process;
	
	-- output the signals
	fsync_o <= fs;
end behavioral;