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
-- RS232 command decoder
-- (c) 2023-2025 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/OpenX32
--
-- This file contains a RS232 command-decoder with error-check.
-- It is like a signal demultiplexer, that will convert serial-data to parallel-data

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity rs232_decoder is 
	generic (
		NUM_OUTPUT_PORTS	: integer := 112;	-- 16x Analog-Output, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40 DSP-Input
		NUM_DSP_CHANNELS	: integer := 40;
		ADDR_WIDTH        : integer := 7    -- log2(112) = 7
	);
	port
	(
		clk				: in std_logic;
		RX_DataReady	: in std_logic;
		RX_Data			: in std_logic_vector(7 downto 0);

		-- deserialized values
		cfg_wr_addr				: out std_logic_vector(ADDR_WIDTH - 1 downto 0);    -- output-channel to be configured
		cfg_wr_data				: out std_logic_vector((ADDR_WIDTH * 8) - 1 downto 0);    -- source for the current output-channel
		cfg_wr_en				: out std_logic
	);
end entity;

architecture Behavioral of rs232_decoder is
	type t_SM_Decoder is (s_Idle, s_CalcSum, s_Check, s_Read);
	signal s_SM_Decoder	: t_SM_Decoder := s_Idle;
	
	signal ErrorCheckWord	: unsigned(15 downto 0);
	signal PayloadSum	: unsigned(15 downto 0);
begin
	process (clk)
		variable b1 : std_logic_vector(7 downto 0);	-- "*" = 0x2A
		variable b2 : std_logic_vector(7 downto 0);	-- C1 = MSB of cmd
		variable b3 : std_logic_vector(7 downto 0);	-- C0 = LSB of cmd
		variable b4 : std_logic_vector(7 downto 0);	-- V7 = MSB of payload
		variable b5 : std_logic_vector(7 downto 0);	-- V6
		variable b6 : std_logic_vector(7 downto 0);	-- V5
		variable b7 : std_logic_vector(7 downto 0);	-- V4
		variable b8 : std_logic_vector(7 downto 0);	-- V3
		variable b9 : std_logic_vector(7 downto 0);	-- V2
		variable b10 : std_logic_vector(7 downto 0);	-- V1
		variable b11 : std_logic_vector(7 downto 0);	-- V0 = LSB of payload
		variable b12 : std_logic_vector(7 downto 0);	-- ErrorCheckWord_MSB
		variable b13 : std_logic_vector(7 downto 0);	-- ErrorCheckWord_LSB
		variable b14 : std_logic_vector(7 downto 0);	-- "#" = 0x23
		
		variable selector: integer range 0 to 65535;
	begin
		if (rising_edge(clk)) then
			if (RX_DataReady = '1' and s_SM_Decoder = s_Idle) then
				-- state 0 -> collect data
				
				cfg_wr_en <= '0';
			
				-- move all bytes forward by one byte and put recent byte at b14
				b1 := b2;
				b2 := b3;
				b3 := b4;
				b4 := b5;
				b5 := b6;
				b6 := b7;
				b7 := b8;
				b8 := b9;
				b9 := b10;
				b10 := b11;
				b11 := b12;
				b12 := b13;
				b13 := b14;
				b14 := RX_Data;

				s_SM_Decoder <= s_CalcSum;
			elsif s_SM_Decoder = s_CalcSum then
				-- build sum of payload and create ErrorCheckWord
				PayloadSum <= unsigned("00000000" & b4) + unsigned("00000000" & b5) + unsigned("00000000" & b6) + unsigned("00000000" & b7) + unsigned("00000000" & b8) + unsigned("00000000" & b9) + unsigned("00000000" & b10) + unsigned("00000000" & b11);

				ErrorCheckWord <= unsigned(b12 & b13);

				s_SM_Decoder <= s_Check;
			elsif s_SM_Decoder = s_Check then
				-- check if we have valid payload
				
				if ((unsigned(b1) = 42) and (unsigned(b14) = 35) and (PayloadSum = ErrorCheckWord)) then
					-- we have valid payload-data -> go into next state
					s_SM_Decoder <= s_Read;
				else
					-- unexpected values -> return to idle
					s_SM_Decoder <= s_Idle;
				end if;
			elsif s_SM_Decoder = s_Read then
				-- write data to output

				selector := to_integer(unsigned(b2 & b3));

				if (selector >= 0 and selector <= 111) then
					cfg_wr_addr <= b3(ADDR_WIDTH - 1 downto 0);
					cfg_wr_data <= b4(6 downto 0) & b5(6 downto 0) & b6(6 downto 0) & b7(6 downto 0) & b8(6 downto 0) & b9(6 downto 0) & b10(6 downto 0) & b11(6 downto 0);
					cfg_wr_en <= '1';
					
				--elsif (selector >= 200 and selector <= 240) then
					-- volume-information
				--	volume_left(((selector - 200) * 8) + 7 downto ((selector - 200) * 8)) <= b11; -- volume uses 8-bit
				--	volume_right(((selector - 200) * 8) + 7 downto ((selector - 200) * 8)) <= b10; -- volume uses 8-bit
				--elsif (selector = 1000) then
					-- set individual 64-bit value
					--value6(63 downto 56) <= b4;
					--value6(55 downto 48) <= b5;
					--value6(47 downto 40) <= b6;
					--value6(39 downto 32) <= b7;
					--value6(31 downto 24) <= b8;
					--value6(23 downto 16) <= b9;
					--value6(15 downto 8) <= b10;
					--value6(7 downto 0) <= b11;
				end if;

				-- we are done -> return to state 0
				s_SM_Decoder <= s_Idle;
			end if;
		end if;
	end process;
end Behavioral;
