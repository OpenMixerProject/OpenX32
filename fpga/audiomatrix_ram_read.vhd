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
-- https://github.com/OpenMixerProject/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram_read is
	generic (
		DATA_WIDTH				: integer := 24;	-- 24-bit audio-samples
		NUM_OUTPUT_PORTS		: integer := 112	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input
	);
	port (
		clk						: in std_logic;
		sync_in					: in std_logic;
		i_ram_data				: in std_logic_vector(DATA_WIDTH - 1 downto 0);
		
		output_channel_idx	: out std_logic_vector(6 downto 0); -- log2(112) = 7 bit
		output_data				: out std_logic_vector(NUM_OUTPUT_PORTS * DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix_ram_read;

architecture behavioral of audiomatrix_ram_read is
	type t_SM_matrix is (s_Idle, s_Read, s_ReadLast);
	signal r_SM_matrix					: t_SM_matrix := s_Idle;

	signal output_channel_idx_next	: integer range 0 to NUM_OUTPUT_PORTS;
	signal pOutput							: integer range 0 to NUM_OUTPUT_PORTS * DATA_WIDTH;
	signal readDelayCounter				: integer range 0 to 2;
begin
	-- as the routing allows routing of input-channel 112 to output-channel 1, we have to write all audio-data
	-- to block-ram, before we start the read-process
	process(clk)
	begin
		if rising_edge(clk) then
			if (r_SM_matrix = s_Idle) then
				if (sync_in = '1') then
					-- set read-address for first read-operation
					output_channel_idx <= "0000000"; -- start at output-channel 1 of 112
					output_channel_idx_next <= 1; -- preload to next address

					pOutput <= 0; -- preload to first output-data
					readDelayCounter <= 0; -- reset readDelayCounter
				
					r_SM_matrix <= s_Read;
				end if;
			
			elsif (r_SM_matrix = s_Read) then
				-- set read-address for next read-operation
				output_channel_idx <= std_logic_vector(to_unsigned(output_channel_idx_next, 7));

				-- increase read-pointer until end
				if (output_channel_idx_next < (NUM_OUTPUT_PORTS - 1)) then
					output_channel_idx_next <= output_channel_idx_next + 1;
				end if;


				if (readDelayCounter = 0) then
					-- cfg-RAM is set
					readDelayCounter <= readDelayCounter + 1;
				elsif (readDelayCounter = 1) then
					-- audio-RAM is set
					readDelayCounter <= readDelayCounter + 1;
				elsif (readDelayCounter = 2) then
					-- read data from RAM
					output_data(pOutput + DATA_WIDTH - 1 downto pOutput) <= i_ram_data;

					-- check if we are close to the last channel
					if (pOutput < ((NUM_OUTPUT_PORTS - 1) * DATA_WIDTH)) then
						pOutput <= pOutput + DATA_WIDTH;

						-- stay in this state
						r_SM_matrix <= s_Read;
					else
						-- we reached the last element
						r_SM_matrix <= s_ReadLast;
					end if;
				end if;
			elsif (r_SM_matrix = s_ReadLast) then
				-- read last data from RAM
				output_data(output_data'left downto output_data'left - DATA_WIDTH + 1) <= i_ram_data;
				
				-- go into idle-state
				r_SM_matrix <= s_Idle;
			end if;
		end if;
	end process;
end architecture behavioral;
