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
-- This file contains a TDM-receiver
-- TDM (Time Division Multiplexed Audio Interface) sends 8 24-bit audio-samples
-- plus 8 additional zero-padding bits right after each other.
-- It uses two additional signals: LR-Clock (Frame Sync) and the serial-clock (bit-clock)
--
-- More information have look at https://gab.wallawalla.edu/~larry.aamodt/engr432/cirrus_logic_TDM_AN301.pdf
--
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|                                                                                      |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- TRIGGER      ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
-- BITCOUNT                                28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                      ^ Rise of FrameSync: reset bit_cnt
--                                                          ^ copy received channel 8 to data_out
--                                                                                              ^ reset bit_cnt
--                                                                                                  ^ copy received channel 1 to data_out

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_rx is
	port (
		bclk			: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync			: in std_logic; -- Frame sync (for X32 it is 384 kHz)
		sdata			: in std_logic; -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
		
		data_out		: out std_logic_vector(23 downto 0) -- received audio-sample
	  );
end tdm_rx;

architecture rtl of tdm_rx is
	signal zfsync				: std_logic;
	signal bit_cnt				: integer range 0 to 31 := 0;
	signal sample_data		: std_logic_vector(31 downto 0) := (others => '0');
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- continuously reading bit into shift-register
			sample_data <= sample_data(sample_data'left - 1 downto 0) & sdata;
			
			-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
			if (fsync = '1' and zfsync = '0') or (bit_cnt = 31) then
				-- reading LSB of current channel

				-- bit_cnt is now 31 and has to be resettet to 0 to
				-- set bit_cnt for next MSB
				bit_cnt <= 0;
			else
				if (bit_cnt = 0) then
					data_out <= sample_data(31 downto 8); -- copy audio-data with 24 bit to output
				end if;

				bit_cnt <= bit_cnt + 1;
			end if;

			zfsync <= fsync;
		end if;
	end process;
end rtl;
