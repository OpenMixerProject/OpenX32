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

entity tdm_ace_demux is
	port (
		bclk				: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync				: in std_logic; -- Frame sync
		tdm0_in			: in std_logic_vector(23 downto 0);
		tdm1_in			: in std_logic_vector(23 downto 0);
		tdm2_in			: in std_logic_vector(23 downto 0);
		tdm3_in			: in std_logic_vector(23 downto 0);
		tdm4_in			: in std_logic_vector(23 downto 0);
		tdm5_in			: in std_logic_vector(23 downto 0);
		tdm6_in			: in std_logic_vector(23 downto 0);
		tdm7_in			: in std_logic_vector(23 downto 0);
		ace_tx_busy		: in std_logic;

		audio_out		: out std_logic_vector(1559 downto 0); -- ACE supports 1 + 64 channels of 24bit audio = 1560 bits
		audio_ready		: out std_logic
	);
end tdm_ace_demux;

architecture rtl of tdm_ace_demux is
	signal zfsync			: std_logic;
	signal zace_tx_busy	: std_logic;
	signal bit_cnt			: integer range 0 to ((8 * 32) - 1) := 0;

	signal packetCounter	: integer range 0 to 15 := 0;
	type lut is array (natural range 0 to 15) of std_logic_vector(7 downto 0);
	constant controlByte:   lut := (
		x"40", x"44", x"48", x"4C",
		x"50", x"54", x"58", x"5C",
		x"60", x"64", x"68", x"6C",
		x"70", x"74", x"78", x"7C");
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check if the EthernetMAC is starting transmitting. If yes, clear the audio-ready-flag
			zace_tx_busy <= ace_tx_busy;
			if ((ace_tx_busy = '1') and (zace_tx_busy = '0')) then
				-- rising edge of ace_tx_busy -> clear audio_ready_flag
				audio_ready <= '0';
			end if;
		
			-- check for fsync
			if (fsync = '1' and zfsync = '0') then
				-- output MSB of sample_data on next falling edge
				bit_cnt <= 0;

				-- channel 0 is the control-channel
				audio_out(7 downto 0) <= controlByte(packetCounter);
				audio_out(23 downto 8) <= "0000000000000000";
				if (packetCounter < 15) then
					packetCounter <= packetCounter + 1;
				else
					packetCounter <= 0;
				end if;

				-- copy channel-data to output-register
				audio_out(47  downto 24)  <= tdm0_in; -- channel 1 (ACE1)
				audio_out(71  downto 48)  <= tdm1_in; -- channel 9 (ACE9)
				audio_out(95  downto 72)  <= tdm2_in; -- channel 17 (ACE17)
				audio_out(119 downto 96)  <= tdm3_in; -- channel 25 (ACE25)
				audio_out(143 downto 120) <= tdm4_in; -- channel 33 (ACE33)
				audio_out(167 downto 144) <= tdm5_in; -- channel 41 (ACE41)
				audio_out(191 downto 168) <= tdm6_in; -- channel 49 (ACE49)
				audio_out(215 downto 192) <= tdm7_in; -- channel 57 (ACE57)
			else
				bit_cnt <= bit_cnt + 1;
			end if;
			
			if (bit_cnt = (31)) then
				-- copy channel-data to output-register
				audio_out(47  + 192 downto 24  + 192) <= tdm0_in; -- channel 2
				audio_out(71  + 192 downto 48  + 192) <= tdm1_in; -- channel 10
				audio_out(95  + 192 downto 72  + 192) <= tdm2_in; -- channel 18
				audio_out(119 + 192 downto 96  + 192) <= tdm3_in; -- channel 26
				audio_out(143 + 192 downto 120 + 192) <= tdm4_in; -- channel 34
				audio_out(167 + 192 downto 144 + 192) <= tdm5_in; -- channel 42
				audio_out(191 + 192 downto 168 + 192) <= tdm6_in; -- channel 50
				audio_out(215 + 192 downto 192 + 192) <= tdm7_in; -- channel 58
			elsif (bit_cnt = (2 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*2 downto 24  + 192*2) <= tdm0_in;
				audio_out(71  + 192*2 downto 48  + 192*2) <= tdm1_in;
				audio_out(95  + 192*2 downto 72  + 192*2) <= tdm2_in;
				audio_out(119 + 192*2 downto 96  + 192*2) <= tdm3_in;
				audio_out(143 + 192*2 downto 120 + 192*2) <= tdm4_in;
				audio_out(167 + 192*2 downto 144 + 192*2) <= tdm5_in;
				audio_out(191 + 192*2 downto 168 + 192*2) <= tdm6_in;
				audio_out(215 + 192*2 downto 192 + 192*2) <= tdm7_in;
			elsif (bit_cnt = (3 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*3 downto 24  + 192*3) <= tdm0_in;
				audio_out(71  + 192*3 downto 48  + 192*3) <= tdm1_in;
				audio_out(95  + 192*3 downto 72  + 192*3) <= tdm2_in;
				audio_out(119 + 192*3 downto 96  + 192*3) <= tdm3_in;
				audio_out(143 + 192*3 downto 120 + 192*3) <= tdm4_in;
				audio_out(167 + 192*3 downto 144 + 192*3) <= tdm5_in;
				audio_out(191 + 192*3 downto 168 + 192*3) <= tdm6_in;
				audio_out(215 + 192*3 downto 192 + 192*3) <= tdm7_in;
			elsif (bit_cnt = (4 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*4 downto 24  + 192*4) <= tdm0_in;
				audio_out(71  + 192*4 downto 48  + 192*4) <= tdm1_in;
				audio_out(95  + 192*4 downto 72  + 192*4) <= tdm2_in;
				audio_out(119 + 192*4 downto 96  + 192*4) <= tdm3_in;
				audio_out(143 + 192*4 downto 120 + 192*4) <= tdm4_in;
				audio_out(167 + 192*4 downto 144 + 192*4) <= tdm5_in;
				audio_out(191 + 192*4 downto 168 + 192*4) <= tdm6_in;
				audio_out(215 + 192*4 downto 192 + 192*4) <= tdm7_in;
			elsif (bit_cnt = (5 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*5 downto 24  + 192*5) <= tdm0_in;
				audio_out(71  + 192*5 downto 48  + 192*5) <= tdm1_in;
				audio_out(95  + 192*5 downto 72  + 192*5) <= tdm2_in;
				audio_out(119 + 192*5 downto 96  + 192*5) <= tdm3_in;
				audio_out(143 + 192*5 downto 120 + 192*5) <= tdm4_in;
				audio_out(167 + 192*5 downto 144 + 192*5) <= tdm5_in;
				audio_out(191 + 192*5 downto 168 + 192*5) <= tdm6_in;
				audio_out(215 + 192*5 downto 192 + 192*5) <= tdm7_in;
			elsif (bit_cnt = (6 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*6 downto 24  + 192*6) <= tdm0_in;
				audio_out(71  + 192*6 downto 48  + 192*6) <= tdm1_in;
				audio_out(95  + 192*6 downto 72  + 192*6) <= tdm2_in;
				audio_out(119 + 192*6 downto 96  + 192*6) <= tdm3_in;
				audio_out(143 + 192*6 downto 120 + 192*6) <= tdm4_in;
				audio_out(167 + 192*6 downto 144 + 192*6) <= tdm5_in;
				audio_out(191 + 192*6 downto 168 + 192*6) <= tdm6_in;
				audio_out(215 + 192*6 downto 192 + 192*6) <= tdm7_in;
			elsif (bit_cnt = (7 * 32 - 1)) then
				-- copy channel-data to output-register
				audio_out(47  + 192*7 downto 24  + 192*7) <= tdm0_in;
				audio_out(71  + 192*7 downto 48  + 192*7) <= tdm1_in;
				audio_out(95  + 192*7 downto 72  + 192*7) <= tdm2_in;
				audio_out(119 + 192*7 downto 96  + 192*7) <= tdm3_in;
				audio_out(143 + 192*7 downto 120 + 192*7) <= tdm4_in;
				audio_out(167 + 192*7 downto 144 + 192*7) <= tdm5_in;
				audio_out(191 + 192*7 downto 168 + 192*7) <= tdm6_in;
				audio_out(215 + 192*7 downto 192 + 192*7) <= tdm7_in;
				
				-- set "audio-ready"-flag and start transmitting using EthernetMAC
				audio_ready <= '1';
			end if;
			
			zfsync <= fsync;
		end if;
	end process;
end rtl;
