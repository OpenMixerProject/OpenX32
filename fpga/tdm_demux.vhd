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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_demux is
	port (
		bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync		: in std_logic; -- Frame sync
		data_in	: in std_logic_vector(23 downto 0);

		ch1_out	: out std_logic_vector(23 downto 0);
		ch2_out	: out std_logic_vector(23 downto 0);
		ch3_out	: out std_logic_vector(23 downto 0);
		ch4_out	: out std_logic_vector(23 downto 0);
		ch5_out	: out std_logic_vector(23 downto 0);
		ch6_out	: out std_logic_vector(23 downto 0);
		ch7_out	: out std_logic_vector(23 downto 0);
		ch8_out	: out std_logic_vector(23 downto 0)
	);
end tdm_demux;

architecture rtl of tdm_demux is
	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to ((8 * 32) - 1) := 0;
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check for fsync
			if (fsync = '1' and zfsync = '0') then
				-- output MSB of sample_data on next falling edge
				bit_cnt <= 0;

				-- copy channel-data to output-register
				ch1_out <= data_in;
			else
				bit_cnt <= bit_cnt + 1;
			end if;
			
			if (bit_cnt = (31)) then
				-- copy channel-data to output-register
				ch2_out <= data_in;
			elsif (bit_cnt = (2 * 32 - 1)) then
				-- copy channel-data to output-register
				ch3_out <= data_in;
			elsif (bit_cnt = (3 * 32 - 1)) then
				-- copy channel-data to output-register
				ch4_out <= data_in;
			elsif (bit_cnt = (4 * 32 - 1)) then
				-- copy channel-data to output-register
				ch5_out <= data_in;
			elsif (bit_cnt = (5 * 32 - 1)) then
				-- copy channel-data to output-register
				ch6_out <= data_in;
			elsif (bit_cnt = (6 * 32 - 1)) then
				-- copy channel-data to output-register
				ch7_out <= data_in;
			elsif (bit_cnt = (7 * 32 - 1)) then
				-- copy channel-data to output-register
				ch8_out <= data_in;
			end if;
			
			zfsync <= fsync;
		end if;
	end process;
end rtl;
