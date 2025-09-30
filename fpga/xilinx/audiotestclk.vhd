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
-- VHDL File to control Cirrus Logic CS42438 IC
-- v0.0.1, 24.07.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiotestclk is
	port (
		clk		: in std_logic; -- 122.88 MHz
		o_sclk_a	: out std_logic; -- 24.576 MHz
		o_sclk_b	: out std_logic; -- 12.288 MHz
		o_fs		: out std_logic -- 48 kHz
	);
end entity;

architecture behavioral of audiotestclk is
	signal count_sclk_a	: natural range 0 to 5 := 1;
	signal count_sclk_b	: natural range 0 to 10 := 1;
	signal count_fs		: natural range 0 to 256 := 1;
	signal sclk_a			: std_logic := '0';
	signal sclk_b			: std_logic := '0';
	signal fs				: std_logic := '0';
begin
	process (clk)
	begin
		if rising_edge(clk) then
			-- generate 24.576 MHz clock for UltraNet (not possible with testclock)
			if (count_sclk_a = 3) then -- divide clk by 6 (122.88 MHz -> 24.576 MHz)
				sclk_a <= not sclk_a;
				count_sclk_a <= 1;
			else
				count_sclk_a <= count_sclk_a + 1;
			end if;
		
			-- generate 12.288 MHz clock for TDM8
			if (count_sclk_b = (122880000/(2*12288000))) then -- divide clk by 10 (122.88 MHz -> 12.288 MHz)
				if (sclk_b = '0') then
					-- rising edge of sclk
					sclk_b <= '1';
				else
					-- falling edge of sclk
					sclk_b <= '0';

					-- derive fs from sclk
					if (count_fs = (12288000/(2*48000))) then -- divide sclk by 256 (12.288 MHz -> 48 kHz)
						fs <= not fs;
						count_fs <= 1;
					else
						count_fs <= count_fs + 1;
					end if;
				end if;
				count_sclk_b <= 1;
			else
				count_sclk_b <= count_sclk_b + 1;
			end if;
		end if;
	end process;
	
	-- output the signals
	o_sclk_a <= sclk_a;
	o_sclk_b <= sclk_b;
	o_fs <= fs;
end behavioral;