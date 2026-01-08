-- 
--   ____                  __   ______ ___  
--  / __ \                 \ \ / /___ \__ \ 
-- | |  | |_ __   ___ _ __  \ V /  __) | ) |
-- | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
-- | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
--  \____/| .__/ \___|_| |_/_/ \_\____/____|
--        | |                               
--        |_|                               
-- 
-- OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
-- Copyright 2025 OpenMixerProject
-- https://github.com/OpenMixerProject/OpenX32
-- 
-- This program is free software; you can redistribute it and/or
-- modify it under the terms of the GNU General Public License
-- version 3 as published by the Free Software Foundation.
-- 
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
-- 
--
-- v0.0.1, 08.01.2026
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity wordclock is
	port (
		i_clk		: in std_logic; -- 12.288 MHz
		o_fs		: out std_logic -- 48 kHz
	);
end entity;

architecture behavioral of wordclock is
	signal count_fs		: natural range 0 to 256 := 1;
	signal fs			: std_logic := '0';
begin
	process (i_clk)
	begin
		if falling_edge(i_clk) then
			-- falling edge of clk
			if (count_fs = (12288000/(2*48000))) then -- divide sclk by 256 (12.288 MHz -> 48 kHz)
				fs <= not fs;
				count_fs <= 1;
			else
				count_fs <= count_fs + 1;
			end if;
		end if;
	end process;
	
	-- output the signals
	o_fs <= fs;
end behavioral;