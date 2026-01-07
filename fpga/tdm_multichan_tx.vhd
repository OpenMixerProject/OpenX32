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
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|    MSB ch1                             LSB MSB ch2                                   |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- BITCOUNT                                28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                      ^ read channel-data of ch1              ^ read channel-data of ch2
--                                                        ^ output MSB of ch1                     ^ output MSB of ch2

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_multichan_tx is
	generic (
		TDM_RECEIVER	: integer := 20;	-- 26 TDM8 receiver are connected
		DATA_WIDTH		: integer := 24	-- 24-bit audio-samples
	);
	port (
		bclk			: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync			: in std_logic; -- Frame sync
		data_in		: in std_logic_vector(TDM_RECEIVER * DATA_WIDTH - 1 downto 0);

		tdm_out		: out std_logic_vector(TDM_RECEIVER - 1 downto 0) -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
	);
end tdm_multichan_tx;

architecture rtl of tdm_multichan_tx is
	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to 31 := 0;
	
	type t_tx_array is array (0 to TDM_RECEIVER - 1) of std_logic_vector(31 downto 0);
	signal sample_buffers : t_tx_array := (others => (others => '0'));
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check for fsync
			if (fsync = '1' and zfsync = '0') or (bit_cnt = 31) then
				-- load channel-data of next channel to output-register
				for i in 0 to TDM_RECEIVER - 1 loop
					sample_buffers(i) <= data_in((i * DATA_WIDTH) + (DATA_WIDTH - 1) downto (i * DATA_WIDTH)) & "00000000";
				end loop;

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
			for i in 0 to TDM_RECEIVER - 1 loop
				tdm_out(i) <= sample_buffers(i)(31 - bit_cnt);
			end loop;
		end if;
	end process;
end rtl;
