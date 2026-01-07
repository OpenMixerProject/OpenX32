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
-- Copyright 2025-2026 OpenMixerProject
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
-- VHDL File to control Cirrus Logic CS2000 PLL IC
-- v0.0.2, 04.01.2026
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audioclk is
	port (
		fs_x_1024_i		: in std_logic; -- 49.152 MHz
		fs_x_512_o		: out std_logic; -- 24.576 MHz
		fs_x_256_o		: out std_logic; -- 12.288 MHz
		fs_o				: out std_logic -- 48 kHz
	);
end entity;

architecture behavioral of audioclk is
	signal clk_a			: std_logic := '0';
	signal clk_b			: std_logic := '0';
	signal count_fs		: natural range 0 to 256 := 1;
	signal fs				: std_logic := '0';
begin
	process (fs_x_1024_i)
	begin
		if rising_edge(fs_x_1024_i) then
			if (clk_a = '0') then
				-- rising edge of clk_a
				clk_a <= '1';

				if (clk_b = '0') then
					-- rising edge of clk_b
					clk_b <= '1';
				else
					-- falling edge of clk_b
					clk_b <= '0';
					
					if (count_fs = (12288000/(2*48000))) then -- divide sclk by 256 (12.288 MHz -> 48 kHz)
						fs <= not fs;
						count_fs <= 1;
					else
						count_fs <= count_fs + 1;
					end if;
					
				end if;
			else
				-- falling edge of clk_a
				clk_a <= '0';
			end if;
		end if;
	end process;
	
	-- output the signals
	fs_x_512_o <= clk_a;
	fs_x_256_o <= clk_b;
	fs_o <= fs;
end behavioral;