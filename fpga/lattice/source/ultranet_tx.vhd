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
-------------------------------------------------------------------------------
-- 8-channel UltraNet-Transmitter
-- Based on 2-channel SP/DIF Transmitter by Danny Witberg from ackspace.nl SP/DIF_transmitter_project
-- Kudos to Samuel Tugler (https://blog.thestaticturtle.fr) for his reverse-engineering-work on UltraNet
--
-- We are using the following format: PREAMBLE | 4-bit AUX | 20-bit audio | VUCP
-- V = Validity = 1, U = UserBit = 0, C = ChannelBit, P = Parity
--
-- sending 4 stereo-samples with 48kHz and 24bits Biphase mark encoded through NRZI
-- We are using Biphase-M-Encoding. See http://www.ee.unb.ca/cgi-bin/tervo/encoding.pl?binary=000111000111000111000111&d=1
------------------------------------------------------------------------------- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity ultranet_tx is
	generic(
		FRAME_COUNTER_RESET 	: std_logic_vector(8 downto 0) := "101111111";
		AES3_PREAMBLE_X 		: std_logic_vector(7 downto 0) := "10010011";
		AES3_PREAMBLE_Y 		: std_logic_vector(7 downto 0) := "10010110";
		AES3_PREAMBLE_Z 		: std_logic_vector(7 downto 0) := "10011100"
	);
	port
	(
		bit_clock		: in std_logic; -- 24.576MHz for 8ch at 48K samplerate
		ch1				: in std_logic_vector(23 downto 0);
		ch2				: in std_logic_vector(23 downto 0);
		ch3				: in std_logic_vector(23 downto 0);
		ch4				: in std_logic_vector(23 downto 0);
		ch5				: in std_logic_vector(23 downto 0);
		ch6				: in std_logic_vector(23 downto 0);
		ch7				: in std_logic_vector(23 downto 0);
		ch8				: in std_logic_vector(23 downto 0);

		ultranet_out_p	: out std_logic;
		ultranet_out_m	: out std_logic
	);
end entity;

architecture behavioral of ultranet_tx is
	-- Setup bits for UltraNet
	-- valid-bit: 				'0' if audio sample word is suitable for conversion to an analogue audio signal. Set to '1' for UltraNet
	-- user-status-bit: 		not used in UltraNet
	-- channel-status-bit:		according to AES/EBU specification for each channel 24 bytes are transmitted. 
	constant valid						: std_logic := '1';
	constant user_status				: std_logic_vector(383 downto 0) := (others => '0');
	constant channel_status				: std_logic_vector(383 downto 0) :=
		"00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & 
		"11000000" & "11110011" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & 
		"00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & 
		"00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & 
		"00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & 
		"00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000" & "00000000";

	-- counter signals
	signal bit_counter				: std_logic_vector(5 downto 0) := (others => '0');
	signal frame_counter			: std_logic_vector(8 downto 0) := (others => '0');
	signal channel_counter			: integer range 0 to 7;

	-- temporary signals
	signal parity					: std_logic;
	signal data_in_buffer			: std_logic_vector(23 downto 0);
	signal data_out_buffer			: std_logic_vector(7 downto 0);
	signal data_biphase				: std_logic := '0';
	signal user_status_shift		: std_logic_vector(383 downto 0);
	signal channel_status_shift		: std_logic_vector(383 downto 0);
begin
	
	bit_clock_counter : process (bit_clock)
	begin
		if bit_clock'event and bit_clock = '1' then
			bit_counter <= bit_counter + 1;
		end if;
	end process bit_clock_counter;

	data_latch : process (bit_clock)
	begin
		if bit_clock'event and bit_clock = '1' then
			parity <= data_in_buffer(23) xor data_in_buffer(22) xor data_in_buffer(21) xor data_in_buffer(20) xor data_in_buffer(19) xor data_in_buffer(18) xor data_in_buffer(17)  xor data_in_buffer(16) xor data_in_buffer(15) xor data_in_buffer(14) xor data_in_buffer(13) xor data_in_buffer(12) xor data_in_buffer(11) xor data_in_buffer(10) xor data_in_buffer(9) xor data_in_buffer(8) xor data_in_buffer(7) xor data_in_buffer(6) xor data_in_buffer(5) xor data_in_buffer(4) xor data_in_buffer(3) xor data_in_buffer(2) xor data_in_buffer(1) xor data_in_buffer(0) xor valid xor user_status_shift(383) xor channel_status_shift(383);

			if bit_counter = 3 then
				-- We are near the end of the preamble, load the sound data in the buffer

				-- UltraNet seems to use only 22 bits for audio and the first two LSB for
				-- identifying channel-pairs
				if channel_counter = 0 then
					data_in_buffer <= ch1(23 downto 2) & "00";
				elsif channel_counter = 1 then
					data_in_buffer <= ch2(23 downto 2) & "00";
				elsif channel_counter = 2 then
					data_in_buffer <= ch3(23 downto 2) & "01";
				elsif channel_counter = 3 then
					data_in_buffer <= ch4(23 downto 2) & "01";
				elsif channel_counter = 4 then
					data_in_buffer <= ch5(23 downto 2) & "10";
				elsif channel_counter = 5 then
					data_in_buffer <= ch6(23 downto 2) & "10";
				elsif channel_counter = 6 then
					data_in_buffer <= ch7(23 downto 2) & "11";
				else
					data_in_buffer <= ch8(23 downto 2) & "11";
				end if;
			end if;

			if bit_counter = 63 then
				-- We are at the 32th bit (2x due to biphase) which means the end of a frame
				
				-- Check if this is the last frame in the audio block
				if frame_counter = FRAME_COUNTER_RESET then
					-- Yes, reset the frame counter
					frame_counter <= (others => '0');
				else
					-- Nope, increment the frame counter
					frame_counter <= frame_counter + 1;
				end if;
			end if;
		end if;
	end process data_latch;

	data_output : process (bit_clock)
	begin
		-- On new bit clock pulse
		if bit_clock'event and bit_clock = '1' then
			if bit_counter = 63 then
				-- We are at the 32th bit of the frame (2x due to biphase) which means the end of a frame
				
				-- Check if this is the last frame in the audio block
				if frame_counter = FRAME_COUNTER_RESET then
					-- Next frame will be the first of the new audio block, load the Z preamble

					channel_counter <= 0;  -- reset channel-counter
					user_status_shift <= user_status;
					channel_status_shift <= channel_status;
					data_out_buffer <= AES3_PREAMBLE_Z;
				else
					-- Next frame is NOT the first of the audio block
					
					-- Check if the frame is even/odd (generally attributed to left/right)
					if frame_counter(0) = '1' then 
						-- Next frame is even, load the X preamble
						data_out_buffer <= AES3_PREAMBLE_X ;
					else 
						-- Next frame is odd, load the Y preamble
						data_out_buffer <= AES3_PREAMBLE_Y;
					end if;

					-- Shift the channel status and user by one to the left
					user_status_shift <= user_status_shift(382 downto 0) & '0';
					channel_status_shift <= channel_status_shift(382 downto 0) & '0';

					-- increment or reset channel-counter
					if (channel_counter < 7) then
						channel_counter <= channel_counter + 1;
					else
						channel_counter <= 0;
					end if;
				end if;
			else
				if bit_counter(2 downto 0) = "111" then -- load new part of data into buffer
					case bit_counter(5 downto 3) is
						when "000" =>
							data_out_buffer <= '1' & data_in_buffer(0) & '1' & data_in_buffer(1) & '1' & data_in_buffer(2) & '1' & data_in_buffer(3);
						when "001" =>
							data_out_buffer <= '1' & data_in_buffer(4) & '1' & data_in_buffer(5) & '1' & data_in_buffer(6) & '1' & data_in_buffer(7);
						when "010" =>
							data_out_buffer <= '1' & data_in_buffer(8) & '1' & data_in_buffer(9) & '1' & data_in_buffer(10) & '1' & data_in_buffer(11);
						when "011" =>
							data_out_buffer <= '1' & data_in_buffer(12) & '1' & data_in_buffer(13) & '1' & data_in_buffer(14) & '1' & data_in_buffer(15);
						when "100" =>
							data_out_buffer <= '1' & data_in_buffer(16) & '1' & data_in_buffer(17) & '1' & data_in_buffer(18) & '1' & data_in_buffer(19);
						when "101" =>
							data_out_buffer <= '1' & data_in_buffer(20) & '1' & data_in_buffer(21) & '1' & data_in_buffer(22) & '1' & data_in_buffer(23);
						when "110" =>
							data_out_buffer <= '1' & valid & '1' & user_status_shift(383) & '1' & channel_status_shift(383) & '1' & parity;
						when others =>
					end case;
				else
					data_out_buffer <= data_out_buffer(6 downto 0) & '0';
				end if;
			end if;
		end if;
	end process data_output;
	
	biphaser : process (bit_clock)
	begin
		if bit_clock'event and bit_clock = '1' then
			if data_out_buffer(data_out_buffer'left) = '1' then
				data_biphase <= not data_biphase;
			end if;
		end if;
	end process biphaser;

	ultranet_out_p <= data_biphase;
	ultranet_out_m <= not data_biphase;
	
end behavioral;
