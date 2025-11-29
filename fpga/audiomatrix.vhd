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

entity audiomatrix is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		NUM_OUTPUT_PORTS	: integer := 112	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input
	);
	port (
		clk					: in std_logic;
		sync_in				: in std_logic;
		input_data			: in  std_logic_vector(NUM_INPUT_PORTS * DATA_WIDTH - 1 downto 0);
		select_lines		: in  std_logic_vector(NUM_OUTPUT_PORTS * 8 - 1 downto 0); -- log2(NUM_INPUT_PORTS) = 8, log2(72) = 7, log2(112) = 7 bit
		i_ram_data			: in  std_logic_vector(DATA_WIDTH - 1 downto 0);
		
		o_ram_read_addr	: out unsigned(6 downto 0); -- log2(112) = 7 bit
		o_ram_write_addr	: out unsigned(6 downto 0); -- log2(112) = 7 bit
		o_ram_data			: out std_logic_vector(DATA_WIDTH - 1 downto 0);
		o_ram_wr_en			: out std_logic;
		output_data			: out std_logic_vector(NUM_OUTPUT_PORTS * DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix;

architecture behavioral of audiomatrix is
	type t_SM_matrix is (s_Idle, s_Write, s_Wait, s_StartRead, s_Read, s_ReadLast);
	signal r_SM_matrix : t_SM_matrix := s_Idle;

	signal pRam				: integer range 0 to NUM_INPUT_PORTS;
	signal pInput			: integer range 0 to NUM_INPUT_PORTS * DATA_WIDTH;
	signal pSelect			: integer range 0 to NUM_INPUT_PORTS * 8;
	signal pOutput			: integer range 0 to NUM_OUTPUT_PORTS * DATA_WIDTH;
begin
	-- as the routing allows routing of input-channel 112 to output-channel 1, we have to write all audio-data
	-- to block-ram, before we start the read-process
	process(clk)
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and r_SM_matrix = s_Idle) then
				o_ram_write_addr <= to_unsigned(0, 7); -- address of first RAM-element
				o_ram_data <= input_data(DATA_WIDTH - 1 downto 0); -- first input-channel
				o_ram_wr_en <= '1';

				pRam <= 1; -- preload to next write-address
				pInput <= DATA_WIDTH; -- preload to next audio-sample					
				r_SM_matrix <= s_Write;
			
			elsif (r_SM_matrix = s_Write) then
				o_ram_write_addr <= to_unsigned(pRam, 7);
				o_ram_data <= input_data(pInput + DATA_WIDTH - 1 downto pInput);

				if (pRam < NUM_INPUT_PORTS - 1) then
					-- increment for next write
					pRam <= pRam + 1;
					pInput <= pInput + DATA_WIDTH;
					
					-- stay in this state
					r_SM_matrix <= s_Write;
				else
					-- go to Wait-State
					r_SM_matrix <= s_Wait;
				end if;

			elsif (r_SM_matrix = s_Wait) then
				-- disable write
				o_ram_wr_en <= '0';

				-- start read-process
				r_SM_matrix <= s_StartRead;

			elsif (r_SM_matrix = s_StartRead) then
				-- set read-address for first read-operation
				o_ram_read_addr <= unsigned(select_lines(6 downto 0)); -- we are taking only 7-bit out of this 8-bit value

				pSelect <= 8; -- preload to next address address
				pOutput <= 0; -- preload to first output-data
			
				r_SM_matrix <= s_Read;

			elsif (r_SM_matrix = s_Read) then
				-- read data from RAM
				output_data(pOutput + DATA_WIDTH - 1 downto pOutput) <= i_ram_data;
				
				-- set read-address for next read-operation
				o_ram_read_addr <= unsigned(select_lines(pSelect + 6 downto pSelect)); -- we are taking only 7-bit out of this 8-bit value

				if (pSelect = (NUM_OUTPUT_PORTS - 1) * 8) then
					-- we reached the last element
					r_SM_matrix <= s_ReadLast;
				else
					-- increase pointers
					pSelect <= pSelect + 8;
					pOutput <= pOutput + DATA_WIDTH;
					
					-- stay in this state
					r_SM_matrix <= s_Read;
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
