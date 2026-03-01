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
-- This logic takes 26 clocks to write the channels of all 26 TDM8 receivers into the RAM
-- 2 additional clocks are taken for cleaning up the statemachine
-- in total it takes maximum of 30 cycles. At 24.576MHz this means (30/24.576MHz) = 1.22 microseconds.
-- Every (32/12.288MHz) = 2.6 microseconds new audio-samples will arrive, so we are safe for 48kHz
--
--
--
-- This RAM-Write-Logic uses a faster clock-domain to read all 26 TDM8 receivers one by one when the DATAVALID of one of the TDM8-receivers is asserted
-- TDM_RX receives the serial audio data and outputs like this:
--                                                     ________________________________________________________________________________...___
-- FRAMESYNC    ______________________________________|                                                                                      |____...
--               _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
-- BITCLOCK     | |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| ...
-- TRIGGER      ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram_write is
	generic (
		TDM_RECEIVER		: integer := 20;	-- 26 TDM8 receiver are connected when both AES50 ports are used
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 160;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40x DSP-Output, 48x AES50A (, 48x AES50B)
		ADDR_WIDTH        : integer := 8    -- log2(208) = 8
	);
	port (
		clk					: in std_logic; -- some clock faster than 12.288MHz bit-clock (e.g. 24.576MHz or even 49.152MHz
		bclk					: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync					: in std_logic; -- on rising edge we reset to channel 1 of 8
		input_data			: in std_logic_vector(TDM_RECEIVER * DATA_WIDTH - 1 downto 0); -- we are receiving one audio-channel every 2.6 microseconds from all TDM8 receivers
		
		o_ram_write_addr	: out std_logic_vector(ADDR_WIDTH - 1 downto 0); -- log2(208) = 8 bit
		o_ram_data			: out std_logic_vector(DATA_WIDTH - 1 downto 0);
		o_ram_wr_en			: out std_logic
	);
end entity audiomatrix_ram_write;



architecture behavioral of audiomatrix_ram_write is
	type t_SM_matrix is (s_Idle, s_Write, s_End);
	signal r_SM_matrix : t_SM_matrix := s_Idle;

	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to 31 := 0;
	signal ch_cnt			: integer range 1 to 8 := 1;
	
	signal pRam				: integer range 0 to NUM_INPUT_PORTS + 8; -- RAM-pointer
	signal pInput			: integer range 0 to NUM_INPUT_PORTS * DATA_WIDTH + DATA_WIDTH; -- input-data-pointer
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

	-- as the routing allows routing of input-channel 208 to output-channel 1, we have
	-- to write all audio-data to block-ram, before we start the read-process
	process(clk)
	begin
		if rising_edge(clk) then
			if (r_SM_matrix = s_Idle) then
				-- reset channel-counter on rising edge of fsync
				if (fsync = '1' and zfsync = '0') then
					ch_cnt <= 8; -- start at channel 8 (channel 0 is "audio unconnected / off", so we start counting at "1")
				end if;
				
				-- wait for new channel data. When bit_cnt is 2 we have 30 bits time to write into the RAM until the next channel is ready
				-- caution: we have different clock-domains. bit_cnt = 2 is shortly after bit_cnt 1 in clock-domain "bclk"
				if (bit_cnt = 3) then
					o_ram_write_addr <= std_logic_vector(to_unsigned(ch_cnt, ADDR_WIDTH)); -- (RAM-element 0 is used for "audio unconnected / off")
					o_ram_data <= input_data(DATA_WIDTH - 1 downto 0); -- always start with TDM8 receiver #0 here
					o_ram_wr_en <= '1';

					pRam <= ch_cnt; -- set write-address to current channel. We always start at TDM8-receiver #0 here (with offset 1)
					pInput <= DATA_WIDTH; -- set input-pointer to end of current channel
					
					r_SM_matrix <= s_Write;
				end if;

			elsif (r_SM_matrix = s_Write) then
				o_ram_write_addr <= std_logic_vector(to_unsigned(pRam + 8, ADDR_WIDTH)); -- we are reading data for same channel number but for next TDM8 receiver, so offset by 8
				o_ram_data <= input_data(pInput + DATA_WIDTH - 1 downto pInput);

				if (pRam < NUM_INPUT_PORTS - 1) then
					-- increment for next write-cycle
					pRam <= pRam + 8; -- we are reading data for same channel number but for next TDM8 receiver
					pInput <= pInput + DATA_WIDTH;
					
					-- stay in this state
					r_SM_matrix <= s_Write;
				else
					-- we reached the last element, so go to End-State
					r_SM_matrix <= s_End;
				end if;

			elsif (r_SM_matrix = s_End) then
				-- disable write
				o_ram_wr_en <= '0';

				-- calculate next channel
				if (ch_cnt < 8) then
					ch_cnt <= ch_cnt + 1;
				else
					ch_cnt <= 1;
				end if;
				
				r_SM_matrix <= s_Idle;
			
			end if;
		end if;
	end process;
end architecture behavioral;
