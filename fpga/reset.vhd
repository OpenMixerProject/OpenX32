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

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity reset is
    Port ( clk : in  STD_LOGIC; -- expecting 16 MHz clock
           i_pll_locked : in  STD_LOGIC;
           o_reset : out  STD_LOGIC);
end reset;

architecture Behavioral of reset is
	signal count_clk	: natural range 0 to 100000 := 0;
begin
	process (clk)
	begin
		if rising_edge(clk) then
			if (i_pll_locked = '1') then
				-- wait 5 ms, then reset circuit and go into online state
				if (count_clk < (16000000/200)) then
					-- waiting
					o_reset <= '0';
					count_clk <= count_clk + 1;
				elsif (count_clk = (16000000/200)) then
					-- resetting for 1 clock
					o_reset <= '1';
					count_clk <= count_clk + 1;
				else
					-- online state. Do nothing here and keep this forever
					o_reset <= '0';
				end if;
			else
				-- still waiting for PLL to lock
				count_clk <= 0;
				o_reset <= '0';
			end if;
		end if;
	end process;
end Behavioral;

