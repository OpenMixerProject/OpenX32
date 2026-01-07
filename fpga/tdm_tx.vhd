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
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|                                                                                      |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- BITCOUNT                                28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                      ^ read channel-data                     ^ read channel-data
--                                                        ^ output first bit                      ^ output first bit

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_tx is
	port (
		bclk			: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync			: in std_logic; -- Frame sync
		data_in		: in std_logic_vector(23 downto 0);

		sdata			: out std_logic -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
	);
end tdm_tx;

architecture rtl of tdm_tx is
	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to 31 := 0;
	signal sample_data	: std_logic_vector(31 downto 0) := (others => '0');
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check for fsync
			if (fsync = '1' and zfsync = '0') or (bit_cnt = 31) then
				-- load channel-data of next channel to output-register
				sample_data <= data_in & "00000000";

				-- bit_cnt is now 31 and has to be resettet to 0 to
				-- output MSB of sample_data on next falling edge
				bit_cnt <= 0;
			else
				bit_cnt <= bit_cnt + 1;
			end if;
			
			zfsync <= fsync;
		end if;
	end process;

	process(bclk)
	begin
		if falling_edge(bclk) then
			sdata <= sample_data(sample_data'left - bit_cnt);
		end if;
	end process;
end rtl;
