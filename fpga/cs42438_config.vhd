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
-- VHDL File to configure Cirrus Logic CS42438 AD/DA IC
-- v0.0.1, 24.07.2025
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
-- 
-- 
-- Recommended process for reset and power-up in Software Mode
-- 1. nRST = 0 -> nRST = 1
-- 2. Config1: Write to Power Control Register and set bit 0 to '1'
-- 3. Config2: Write to MISC register
-- 4. Config3: Mute all DACs
-- 5. Config4: Write to Power Control Register and set bit 0 to '0'
-- 6. Wait around 2000 LRCK (=50ms)
-- 7. Config5: Unmute DACs
-- 


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cs42438_config is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_start		: in std_logic;
		i_txbusy		: in std_logic;
		
		o_nRST		: out std_logic;
		o_address	: out std_logic_vector(7 downto 0);
		o_map			: out std_logic_vector(7 downto 0);
		o_data		: out std_logic_vector(7 downto 0);
		o_start		: out std_logic
	);
end entity;

architecture behavioral of cs42438_config is
	type t_SM is (s_Startup, s_Reset, s_Start, s_Config1, s_Wait1, s_Config2, s_Wait2, s_Config3, s_Wait3, s_Config4, s_Wait4, s_Config5, s_Wait5, s_Done);
	signal s_SM				: t_SM := s_Startup;
	signal count_state	: natural range 0 to 1000000 := 0; -- allow up to 62ms
	
	signal mapaddress		: std_logic_vector(7 downto 0);
	signal data				: std_logic_vector(7 downto 0);
	signal start			: std_logic;
	
	constant chipaddress	: std_logic_vector(7 downto 0) := "10011110"; -- 7-bit address followed by R/nW
	constant map_power	: std_logic_vector(7 downto 0) := x"02"; -- Power Control Register
	constant map_misc		: std_logic_vector(7 downto 0) := x"04"; -- Misc Control Register
	constant map_mute		: std_logic_vector(7 downto 0) := x"07"; -- Channel Mute Register
begin
	process (clk)
	begin
		if rising_edge(clk) then
			if (i_start = '1') then
				-- start the configuration process
				o_nRST <= '1';
				count_state <= 0;
				mapaddress <= x"00";
				data <= x"00";
				start <= '0';
				s_SM <= s_Reset;
				
			else
				if (s_SM = s_Startup) then
					-- wait for begin
					
				elsif (s_SM = s_Reset) then
					o_nRST <= '0';

					-- keep this state and leave it after 1.5ms
					if (count_state = (16000000/650)) then
						count_state <= 0;
						s_SM <= s_Start;
					else
						count_state <= count_state + 1;
					end if;

				elsif (s_SM = s_Start) then
					-- release reset
					o_nRST <= '1';

					-- keep this state and leave it after 1.5ms
					if (count_state = (16000000/650)) then
						s_SM <= s_Config1;
					else
						count_state <= count_state + 1;
					end if;

				elsif (s_SM = s_Config1) then

					-- transmit bits over SPI-interface
					mapaddress <= map_power;
					data <= "00000001"; -- power down device
					start <= '1';
					count_state <= 0;
					
					s_SM <= s_Wait1;

				elsif (s_SM = s_Wait1) then
					start <= '0';
					
					-- wait until spi message has been sent
					if (i_txbusy = '0') then
						-- keep this state and leave it after 1.5us
						if (count_state = (16000000/650000)) then
							s_SM <= s_Config2;
						else
							count_state <= count_state + 1;
						end if;
					end if;
					
				elsif (s_SM = s_Config2) then

					-- transmit bits over SPI-interface
					mapaddress <= map_misc;
					data <= "01110110"; -- config misc register: FREEZE | AUX_DIF | 5..0 = RESERVED. AUX_DIF=0=LeftJustified, AUX_DIF=1=I2S
					start <= '1';
					count_state <= 0;
					
					s_SM <= s_Wait2;

				elsif (s_SM = s_Wait2) then
					start <= '0';
					
					-- wait until spi message has been sent
					if (i_txbusy = '0') then
						-- keep this state and leave it after 1.5us
						if (count_state = (16000000/650000)) then
							s_SM <= s_Config3;
						else
							count_state <= count_state + 1;
						end if;
					end if;
					
				elsif (s_SM = s_Config3) then

					-- transmit bits over SPI-interface
					mapaddress <= map_mute;
					data <= "11111111"; -- config mute register: mute all DACs
					start <= '1';
					count_state <= 0;
					
					s_SM <= s_Wait3;

				elsif (s_SM = s_Wait3) then
					start <= '0';
					
					-- wait until spi message has been sent
					if (i_txbusy = '0') then
						-- keep this state and leave it after 1.5us
						if (count_state = (16000000/650000)) then
							s_SM <= s_Config4;
						else
							count_state <= count_state + 1;
						end if;
					end if;
					
				elsif (s_SM = s_Config4) then

					-- transmit bits over SPI-interface
					mapaddress <= map_power;
					data <= "00000000"; -- power up device
					start <= '1';
					count_state <= 0;
					
					s_SM <= s_Wait4;

				elsif (s_SM = s_Wait4) then
					start <= '0';
					
					-- wait until spi message has been sent
					if (i_txbusy = '0') then
						-- keep this state and leave it after at least 2000 LR-cycles (2000 * 1/48kHz ~ 50ms)
						if (count_state = (16000000/20)) then
							s_SM <= s_Config5;
						else
							count_state <= count_state + 1;
						end if;
					end if;
					
				elsif (s_SM = s_Config5) then

					-- transmit bits over SPI-interface
					mapaddress <= map_mute;
					data <= "00000000"; -- unmute all DACs
					start <= '1';
					
					s_SM <= s_Wait5;

				elsif (s_SM = s_Wait5) then
					start <= '0';
					
					-- wait until spi message has been sent
					if (i_txbusy = '0') then
						s_SM <= s_Done;
					end if;
					
				elsif (s_SM = s_Done) then
					-- stay here until next start-request
					mapaddress <= x"00";
					data <= x"00";
					start <= '0';
					
				end if;		
			end if;
		end if;
	end process;
	
	o_address <= chipaddress;
	o_map <= mapaddress;
	o_data <= data;
	o_start <= start;
end behavioral;