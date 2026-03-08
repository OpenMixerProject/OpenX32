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
	port (
		clk					: in std_logic;	-- expecting 16 MHz clock
		o_pripll_rst		: out std_logic;
		o_secpll_rst		: out std_logic;
		o_global_rst 		: out std_logic;	-- reset-signal 0 (5ms) -> 1 (250ns) -> 0 (1µs) -> start (inf)
		o_online				: out std_logic
	);
end reset;

architecture Behavioral of reset is
	type t_SM is (s_Init, s_Startup, s_StartPriPll, s_WaitForPriPll, s_StartSecPLL, s_WaitForSecPll, s_Reset, s_Wait, s_Done);
	signal s_SM				: t_SM := s_Init;

	signal pripll_rst		: std_logic;
	signal secpll_rst		: std_logic;
	signal global_rst		: std_logic;
	signal online			: std_logic;
	
	signal count_clk		: natural range 0 to 100000 := 0;
	signal wait_a			: natural range 0 to 1000 := 0;
	signal wait_b			: natural range 0 to 1000 := 0;
begin
	process (clk)
	begin
		if rising_edge(clk) then
			-- with 16MHz each clockstep is 62.5 ns
		
			if (s_SM = s_Init) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '0';
				
				wait_a <= 0;
				wait_b <= 0;
				s_SM <= s_Startup;
				
			elsif (s_SM = s_Startup) then
				-- wait for 5ms
				if (wait_b < 80) then
					if (wait_a < 1000) then
						wait_a <= wait_a + 1;
					else
						-- 62.5 us have passed
						wait_a <= 0;
						wait_b <= wait_b + 1;
					end if;
				else
					wait_a <= 0;
					wait_b <= 0;
					
					s_SM <= s_StartPriPll;
				end if;
			
			elsif (s_SM = s_StartPriPll) then
				pripll_rst <= '1';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '0';

				wait_a <= 0;
				wait_b <= 0;
				s_SM <= s_WaitForPriPll;
				
			elsif (s_SM = s_WaitForPriPll) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '0';

				-- wait for 50ms
				if (wait_b < 800) then
					if (wait_a < 1000) then
						wait_a <= wait_a + 1;
					else
						-- 62.5 us have passed
						wait_a <= 0;
						wait_b <= wait_b + 1;
					end if;
				else
					wait_a <= 0;
					wait_b <= 0;
					
					s_SM <= s_StartSecPLL;
				end if;
			
			elsif (s_SM = s_StartSecPLL) then
				pripll_rst <= '0';
				secpll_rst <= '1';
				global_rst <= '0';
				online <= '0';

				wait_a <= 0;
				wait_b <= 0;
				s_SM <= s_WaitForSecPll;
			
			elsif (s_SM = s_WaitForSecPll) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '0';

				-- wait for 50ms
				if (wait_b < 800) then
					if (wait_a < 1000) then
						wait_a <= wait_a + 1;
					else
						-- 62.5 us have passed
						wait_a <= 0;
						wait_b <= wait_b + 1;
					end if;
				else
					wait_a <= 0;
					wait_b <= 0;
					
					s_SM <= s_Reset;
				end if;
			
			elsif (s_SM = s_Reset) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '1';
				online <= '0';
			
				-- keep this state for 62.5 us
				if (wait_a < 1000) then
					wait_a <= wait_a + 1;
				else
					-- 62.5 us have passed
					wait_a <= 0;
					wait_b <= 0;
					
					s_SM <= s_Wait;
				end if;
			
			elsif (s_SM = s_Wait) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '0';

				-- wait for 500ms
				if (wait_b < 800) then
					if (wait_a < 1000) then
						wait_a <= wait_a + 1;
					else
						-- 62.5 us have passed
						wait_a <= 0;
						wait_b <= wait_b + 1;
					end if;
				else
					wait_a <= 0;
					wait_b <= 0;
					
					s_SM <= s_Done;
				end if;

			elsif (s_SM = s_Done) then
				pripll_rst <= '0';
				secpll_rst <= '0';
				global_rst <= '0';
				online <= '1';
				
				-- stay in this state forever

			end if;
		end if;
	end process;

	o_pripll_rst <= pripll_rst;
	o_secpll_rst <= secpll_rst;
	o_global_rst <= global_rst;
	o_online <= online;
end Behavioral;

