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
-- Block-RAM-Module for Audio-Routing-Matrix
-- v0.1.0, 08.08.2025
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram is
	generic (
		DATA_WIDTH				: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS		: integer := 112;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		RAM_DEPTH				: integer := 128; -- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		ADDR_WIDTH         	: integer := 7    -- log2(112) = 7
	);
	port (
		clk						: in std_logic;
		write_addr				: in std_logic_vector(ADDR_WIDTH - 1 downto 0); -- log2(112) = 7
		i_data					: in std_logic_vector(DATA_WIDTH - 1 downto 0);
		wr_en						: in std_logic;

		read_addr				: in std_logic_vector(ADDR_WIDTH - 1 downto 0); -- log2(112) = 7

		o_data					: out std_logic_vector(DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix_ram;

architecture behavioral of audiomatrix_ram is
	type ram_type is array (RAM_DEPTH - 1 downto 0) of std_logic_vector(DATA_WIDTH - 1 downto 0); -- we are using only 112 elements, but we take 2^7 RAM-elements to match the full address-range of 7-bit address-pointers
	signal ram_inst : ram_type;
begin
	process(clk)
	begin
		if rising_edge(clk) then
			-- write audio data to RAM
			if wr_en = '1' then
				ram_inst(to_integer(unsigned(write_addr))) <= i_data;
			end if;

			-- read audio data from RAM
			o_data <= ram_inst(to_integer(unsigned(read_addr)));
		end if;
	end process;
end architecture behavioral;