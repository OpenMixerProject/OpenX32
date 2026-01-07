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
-- VHDL File to route multiple audio-inputs to multiple audio-outputs
-- v0.2.0, 04.01.2026
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
--
-- TDM_RX receives the serial audio data and outputs like this:
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|    MSB ch1                             LSB MSB ch2                                   |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- BITCOUNT                                28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                      ^ Rise fsync detected: bit_cnt <= 0     ^ reset bit_cnt to 0
--                                                          ^ TDM RX copies data for ch8 to output  ^ TDM RX copies data for ch1 to output
--                                                                   ^ read data for channel 8               ^ read data for channel 1
--                                                                   |==== 26 clks = 0.5 us ====|            |==== 26 clks = 0.5 us ====|
--																				          ^ Write TDM#0 channel 8
--																				           ^ Write TDM#1 channel 8
--																				            ...       ^ Write TDM#25
--																						                  ^ Increase Channel-counter
--																						                   ^ go to Idle
-- ram_reader acts like this:
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|    MSB ch1                             LSB MSB ch2                                   |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- BITCOUNT    30  31   0   1   2   3  ... 28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                    ^ set ch_cnt to 1
--                  ^ reset bit_cnt to 0                ^ Rise fsync detected: bit_cnt <= 0     ^ reset bit_cnt to 0
--                           ^ read data for ch1 from RAM        ^ read data for ch2 from RAM            ^ read data for ch3 from RAM
--                           |==== 30 clks = 0.6 us ====|        |==== 30 clks = 0.6 us ====|            |==== 30 clks = 0.6 us ====|
--                                       ^ put data to output                ^ put data to output                    ^ put data to output
--                                        ^ increment ch_cnt                  ^ increment ch_cnt                      ^ increment ch_cnt
--
-- TDM_TX sends the serial audio data and outputs like this:
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|    MSB ch1                             LSB MSB ch2                                   |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- BITCOUNT                                28  29  30  31   0   1   2...16 17  18..28  29  30  31   0   1   2   3...16
--                                                      ^ read channel-data of ch1              ^ read channel-data of ch2
--                                                        ^ output MSB of ch1                     ^ output MSB of ch2

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram_read is
	generic (
		TDM_SENDER			: integer := 20;	-- 26 TDM8 sender are connected
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_OUTPUT_PORTS	: integer := 160;	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input, 48x AES50A, 48x AES50B
		ADDR_WIDTH        : integer := 8    -- log2(208) = 8
	);
	port (
		clk					: in std_logic;
		bclk					: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync					: in std_logic;	-- regular framesync (wordclock)
		ram_data				: in std_logic_vector(DATA_WIDTH - 1 downto 0);
		
		channel_idx_out	: out std_logic_vector(ADDR_WIDTH - 1 downto 0); -- log2(208) = 8 bit
		data_out				: out std_logic_vector(TDM_SENDER * DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix_ram_read;

architecture behavioral of audiomatrix_ram_read is
	type t_SM_matrix is (s_Idle, s_Read, s_IncrementChannel);
	signal r_SM_matrix					: t_SM_matrix := s_Idle;

	signal bit_cnt							: integer range 0 to 31 := 0;
	signal zfsync							: std_logic;
	signal ch_cnt							: integer range 0 to NUM_OUTPUT_PORTS + 8 := 0; -- channel_index within routing-RAM is zero-based (0=ch1, 1=ch2, ...)
	
	signal channel_idx					: integer range 0 to NUM_OUTPUT_PORTS + 8;
	signal tdm_cnt							: integer range 0 to TDM_SENDER;
	signal pOutput							: integer range 0 to NUM_OUTPUT_PORTS * DATA_WIDTH + DATA_WIDTH;
	signal readDelayCounter				: integer range 0 to 3;
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
			if (fsync = '1' and zfsync = '0') or (bit_cnt = 31) then
				bit_cnt <= 0;
			else
				bit_cnt <= bit_cnt + 1;
			end if;

			zfsync <= fsync;
		end if;
	end process;

	-- as the routing allows routing of input-channel 112 to output-channel 1, we have to write all audio-data
	-- to block-ram, before we start the read-process
	process(clk)
	begin
		if rising_edge(clk) then
			if (r_SM_matrix = s_Idle) then
				-- reset channel-counter on rising edge of fsync
				if (fsync = '1' and zfsync = '0') then
					ch_cnt <= 1; -- start with the second channel after rising framesync as reading from RAM takes some clocks
				end if;
				
				-- when bit_cnt is 1 we have 30 bits time to read data for all TDM-senders from RAM
				-- caution: we have different clock-domains. bit_cnt = 2 is shortly after bit_cnt 1 in clock-domain "bclk"
				if (bit_cnt = 2) then
					-- set read-address for first read-operation
					channel_idx_out <= std_logic_vector(to_unsigned(ch_cnt, ADDR_WIDTH)); -- start at current channel-offset
					channel_idx <= ch_cnt + 8;

					tdm_cnt <= 0; -- start with TDM#0
					pOutput <= 0; -- preload to first output-data
					readDelayCounter <= 0; -- reset readDelayCounter
				
					r_SM_matrix <= s_Read;
				end if;
			
			elsif (r_SM_matrix = s_Read) then
				-- for the first run we have to wait for the two RAM-blocks, but after this we can read on each clock

				-- put the desired read-address into the pipeline. The result is ready after 2 waitstates
				if (tdm_cnt < (TDM_SENDER - 1)) then
					-- we haven't reached the data for last TDM-module
					channel_idx_out <= std_logic_vector(to_unsigned(channel_idx, ADDR_WIDTH));
					channel_idx <= channel_idx + 8; -- increase by 8 channels for next TDM8-sender (together with offset of current channel)
				end if;

				-- waitstates to take account for the pipeline-delay
				if (readDelayCounter < 2) then
					-- waitstate 1: routing-RAM is set
					-- waitstate 2: audio-RAM is set
					readDelayCounter <= readDelayCounter + 1;
					
				else
					-- read the data for current channel from RAM beginning with TDM#0 and incrementing to TDM #13
					data_out(pOutput + DATA_WIDTH - 1 downto pOutput) <= ram_data;

					if (tdm_cnt < (TDM_SENDER - 1)) then
						-- we haven't reached the data for last TDM-module
						
						tdm_cnt <= tdm_cnt + 1;
						pOutput <= pOutput + DATA_WIDTH; -- preload for next data
						
						r_SM_matrix <= s_Read;
					else
						r_SM_matrix <= s_IncrementChannel;
					end if;
				end if;
			elsif (r_SM_matrix = s_IncrementChannel) then
				-- calculate next channel
				if (ch_cnt < 7) then
					ch_cnt <= ch_cnt + 1;
				else
					ch_cnt <= 0;
				end if;

				r_SM_matrix <= s_Idle;
				
			end if;
		end if;
	end process;
	
	-- as the routing allows routing of input-channel 112 to output-channel 1, we have to write all audio-data
	-- to block-ram, before we start the read-process
--process(clk)
--begin
--	if rising_edge(clk) then
--		if (r_SM_matrix = s_Idle) then
--			-- reset channel-counter on rising edge of fsync
--			if (fsync = '1' and zfsync = '0') then
--				ch_cnt <= 1; -- start with the second channel after rising framesync as reading from RAM takes some clocks
--			end if;
--			
--			-- when bit_cnt is 1 we have 30 bits time to read data for all TDM-senders from RAM
--			-- caution: we have different clock-domains. bit_cnt = 2 is shortly after bit_cnt 1 in clock-domain "bclk"
--			if (bit_cnt = 2) then
--				-- set read-address for first read-operation
--				channel_idx_out <= std_logic_vector(to_unsigned(ch_cnt, ADDR_WIDTH)); -- start at current channel-offset
--				channel_idx <= (ch_cnt + 8); -- increase by 8 channels for next TDM8-sender (together with offset of current channel)
--
--				pOutput <= 0; -- preload to first output-data
--				readDelayCounter <= 0; -- reset readDelayCounter
--			
--				r_SM_matrix <= s_Read;
--			end if;
--		
--		elsif (r_SM_matrix = s_Read) then
--			-- set read-address for next read-operation on each rising clock, but wait 3 cycles on the first read
--			channel_idx_out <= std_logic_vector(to_unsigned(channel_idx, ADDR_WIDTH));
--
--			-- increase read-pointer until end
--			if (channel_idx < (NUM_OUTPUT_PORTS - 8)) then
--				channel_idx <= channel_idx + 8; -- increase by 8 channels for next TDM8-sender
--			end if;
--
--			-- for the first run we have to wait for the two RAM-blocks, but after this we can read on each clock
--			if (readDelayCounter < 2) then
--				-- waitstates to take account for the pipeline-delay
--			
--				-- waitstate 1: routing-RAM is set
--				-- waitstate 2: audio-RAM is set
--				readDelayCounter <= readDelayCounter + 1;
--			else
--				-- read data from audio-RAM
--				data_out(pOutput + DATA_WIDTH - 1 downto pOutput) <= ram_data;
--
--				-- check if we are close to the last channel
--				if (pOutput < ((NUM_OUTPUT_PORTS - 1) * DATA_WIDTH)) then
--					pOutput <= pOutput + DATA_WIDTH;
--
--					-- stay in this state
--					r_SM_matrix <= s_Read;
--				else
--					-- we reached the last element
--					r_SM_matrix <= s_ReadLast;
--				end if;
--			end if;
--		elsif (r_SM_matrix = s_ReadLast) then
--			-- read last data from RAM
--			data_out(data_out'left downto data_out'left - DATA_WIDTH + 1) <= ram_data;
--
--			-- calculate next channel
--			if (ch_cnt < 7) then
--				ch_cnt <= ch_cnt + 1;
--			else
--				ch_cnt <= 0;
--			end if;
--
--			r_SM_matrix <= s_Idle;
--			
--		end if;
--	end if;
--end process;
end architecture behavioral;
