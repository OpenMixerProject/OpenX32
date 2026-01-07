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
-- This file contains a multichannel TDM-receiver
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
--                                                      ^ Rise fsync detected: bit_cnt <= 0     ^ reset bit_cnt to 0
--                                                          ^ copy received channel 8 to data_out   ^ copy received channel 1 to data_out

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_multichan_rx is
	generic (
		TDM_RECEIVER	: integer := 20;	-- 26 TDM8 receiver are connected
		DATA_WIDTH		: integer := 24	-- 24-bit audio-samples
	);
	port (
		bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync		: in std_logic; -- Frame sync (for X32 it is 384 kHz)
		tdm_in	: in std_logic_vector(TDM_RECEIVER - 1 downto 0); -- 26 individual TDM-inputs

		data_out	: out std_logic_vector(TDM_RECEIVER * DATA_WIDTH - 1 downto 0) -- audio data for RAM-block
	);
end entity;

architecture rtl of tdm_multichan_rx is
	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to 31 := 0;

	type t_data_array is array (0 to TDM_RECEIVER - 1) of std_logic_vector(31 downto 0); -- TDM-interface uses 32 bit
	signal internal_data	: t_data_array := (others => (others => '0'));
	signal shift_regs		: t_data_array := (others => (others => '0'));
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- continuously reading bit into shift-register
			for i in 0 to TDM_RECEIVER - 1 loop
				shift_regs(i) <= shift_regs(i)(30 downto 0) & tdm_in(i);
			end loop;
			
			-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
			if (fsync = '1' and zfsync = '0') or (bit_cnt = 31) then
				-- reading LSB of current channel

				-- bit_cnt is now 31 and has to be resettet to 0 to
				-- set bit_cnt for next MSB
				bit_cnt <= 0;
			else
				if (bit_cnt = 0) then
					internal_data <= shift_regs; -- take data of all 26 channels
				end if;

				bit_cnt <= bit_cnt + 1;
			end if;

			zfsync <= fsync;
		end if;
	end process;
	
	-- map internal data to output
	gen_out: for i in 0 to TDM_RECEIVER - 1 generate
		data_out((i * DATA_WIDTH) + (DATA_WIDTH - 1) downto (i * DATA_WIDTH)) <= internal_data(i)(31 downto (32 - DATA_WIDTH));
	end generate;
end architecture;
