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
-- VHDL File to route multiple audio-inputs to multiple audio-outputs
-- v0.1.0, 08.08.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram_write is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
	);
	port (
		clk					: in std_logic;
		sync_in				: in std_logic;
		input_data			: in std_logic_vector(NUM_INPUT_PORTS * DATA_WIDTH - 1 downto 0);
		
		o_ram_write_addr	: out std_logic_vector(6 downto 0); -- log2(112) = 7 bit
		o_ram_data			: out std_logic_vector(DATA_WIDTH - 1 downto 0);
		o_ram_wr_en			: out std_logic;
		o_write_done		: out std_logic
	);
end entity audiomatrix_ram_write;

architecture behavioral of audiomatrix_ram_write is
	type t_SM_matrix is (s_Idle, s_Write, s_End);
	signal r_SM_matrix : t_SM_matrix := s_Idle;

	signal pRam				: integer range 0 to NUM_INPUT_PORTS; -- RAM-pointer
	signal pInput			: integer range 0 to NUM_INPUT_PORTS * DATA_WIDTH; -- input-data-pointer
begin
	-- as the routing allows routing of input-channel 112 to output-channel 1, we have to write all audio-data
	-- to block-ram, before we start the read-process
	process(clk)
	begin
		if rising_edge(clk) then
			if (r_SM_matrix = s_Idle) then
				if (sync_in = '1') then
					-- we received new audio-data -> copy all input-signals to Block-RAM
				
					o_ram_write_addr <= std_logic_vector(to_unsigned(1, 7)); -- address of first RAM-element (RAM-element 0 is used for "audio unconnected / off")
					o_ram_data <= input_data(DATA_WIDTH - 1 downto 0); -- first input-channel
					o_ram_wr_en <= '1';

					pRam <= 1; -- preload to next write-address
					pInput <= DATA_WIDTH; -- preload to next audio-sample					
					r_SM_matrix <= s_Write; -- enter write-state
				end if;

				-- we are in Idle-state, so set/keep write-done to 0
				o_write_done <= '0';
			
			elsif (r_SM_matrix = s_Write) then
				o_ram_write_addr <= std_logic_vector(to_unsigned(pRam + 1, 7));
				o_ram_data <= input_data(pInput + DATA_WIDTH - 1 downto pInput);

				if (pRam < NUM_INPUT_PORTS - 1) then
					-- increment for next write-cycle
					pRam <= pRam + 1;
					pInput <= pInput + DATA_WIDTH;
					
					-- stay in this state
					r_SM_matrix <= s_Write;
				else
					-- we reached the last element, so go to End-State
					r_SM_matrix <= s_End;
				end if;

			elsif (r_SM_matrix = s_End) then
				-- disable write
				o_ram_wr_en <= '0';
				
				-- set write-done-flag
				o_write_done <= '1';

				-- go into Idle-state
				r_SM_matrix <= s_Idle;

			end if;
		end if;
	end process;
end architecture behavioral;
