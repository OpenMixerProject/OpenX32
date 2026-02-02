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
-- VHDL File to control Cirrus Logic CS2000CP PLL IC
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

entity cs2000cp_config is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_start		: in std_logic;
		i_txbusy		: in std_logic;
		
		o_address	: out std_logic_vector(7 downto 0);
		o_map			: out std_logic_vector(7 downto 0);
		o_data		: out std_logic_vector(7 downto 0);
		o_start		: out std_logic
	);
end entity;

architecture behavioral of cs2000cp_config is
	type t_SM is (s_Startup, s_Config, s_Wait, s_Done);
	signal s_SM				: t_SM := s_Startup;
	signal count_state	: natural range 0 to 50 := 0; -- allow up to 3us
	
	signal mapaddress		: std_logic_vector(7 downto 0);
	signal data				: std_logic_vector(7 downto 0);
	signal start			: std_logic;
	
	signal count_cfg		: natural range 0 to 12 := 0;
	constant chipaddress	: std_logic_vector(7 downto 0) := "10011110"; -- 7-bit address followed by R/nW
	type cs2000_cfg_lut_t is array(0 to 11, 0 to 1) of std_logic_vector(7 downto 0);
	constant cs2000_cfg_lut: cs2000_cfg_lut_t := (
		(x"03", "00000111"), -- AUX-Pin as PLL-Lock + Device-Config Enable 1
		(x"05", "00001001"), -- Freeze-Bit to 1 + Device-Config Enable 2
		(x"17", "00001000"), -- Clock-Output Disabled if PLL Unlocked + High-Accuracy-Mode
		(x"16", "00001000"), -- Clock-Skip Disabled + Ref-Clock DIV to "01" -> 16MHz to 28MHz range
		(x"1E", "00000000"), -- Loop-Bandwidth to 1 Hz
		(x"04", "00000001"), -- Hybrid-PLL mode and lock-clock-ratio to 0
		
		-- ratio for 12.288 MHz clock-output
		-- 16MHz -> 12.288 MHz = x0.768 -> 2^20 * 0.768 =  805306 = 0x000C49BA
		--(x"06", x"00"), -- Multiplicator-Value MSB
		--(x"07", x"0C"), -- Multiplicator-Value
		--(x"08", x"49"), -- Multiplicator-Value
		--(x"09", x"BA"), -- Multiplicator-Value LSB

		-- ratio for 24.576 MHz clock-output
		-- 16MHz -> 24.576 MHz = x1.536 -> 2^20 * 1.536 = 1610613 = 0x00189375
		--(x"06", x"00"), -- Multiplicator-Value MSB
		--(x"07", x"18"), -- Multiplicator-Value
		--(x"08", x"93"), -- Multiplicator-Value
		--(x"09", x"75"), -- Multiplicator-Value LSB

		-- ratio for 45.1584 MHz clock-output (1024 x 44.1kHz)
		-- 16MHz -> 45.1584 MHz = x2.8224 -> 2^20 * 2.8224 = 2959501 = 0x002D288D
		--(x"06", x"00"), -- Multiplicator-Value MSB
		--(x"07", x"2D"), -- Multiplicator-Value
		--(x"08", x"28"), -- Multiplicator-Value
		--(x"09", x"8D"), -- Multiplicator-Value LSB

		-- ratio for 49.152 MHz clock-output (1024 x 48kHz)
		-- 16MHz -> 49.152 MHz = x3.072 -> 2^20 * 3.072 = 3221225 = 0x003126E9
		(x"06", x"00"), -- Multiplicator-Value MSB
		(x"07", x"31"), -- Multiplicator-Value
		(x"08", x"26"), -- Multiplicator-Value
		(x"09", x"E9"), -- Multiplicator-Value LSB

		-- ratio for 98.304 MHz clock-output (1024 x 96kHz)
		-- 16MHz -> 98.304 MHz = x6.144 -> 2^20 * 6.144 = 6442451 = 0x00624DD3
		--(x"06", x"00"), -- Multiplicator-Value MSB
		--(x"07", x"62"), -- Multiplicator-Value
		--(x"08", x"4D"), -- Multiplicator-Value
		--(x"09", x"D3"), -- Multiplicator-Value LSB

		(x"02", "00000000"), -- Enable CLK- and AUX-Output
		(x"05", "00000001")  -- Set Freeze to 0 -> changes take effect immediately
	);
begin
	process (clk)
	begin
		if rising_edge(clk) then
			if (s_SM = s_Startup) then
				-- wait for begin
				if (i_start = '1') then
					-- start the configuration process
					mapaddress <= x"00";
					data <= x"00";
					start <= '0';
					count_cfg <= 0;
					s_SM <= s_Config;
				end if;

			elsif (s_SM = s_Config) then

				-- transmit bits over SPI-interface
				mapaddress <= cs2000_cfg_lut(count_cfg, 0); -- set map address
				data <= cs2000_cfg_lut(count_cfg, 1); -- set the data
				start <= '1';
				count_state <= 0;
				
				s_SM <= s_Wait;

			elsif (s_SM = s_Wait) then
				start <= '0';
				
				-- wait until spi message has been sent
				if (i_txbusy = '0') then
					-- keep this state and leave it after 1.5us
					if (count_state = (16000000/650000)) then

						-- check if we have reached end of configuration
						if (count_cfg = 11) then
							-- yes -> end configuration
							s_SM <= s_Done;
						else
							-- still data to transmit
							count_cfg <= count_cfg + 1;
							s_SM <= s_Config;
						end if;
					else
						count_state <= count_state + 1;
					end if;
				end if;
									
			elsif (s_SM = s_Done) then
				-- stay here until next start-request
				mapaddress <= x"00";
				data <= x"00";
				count_cfg <= 0;
				start <= '0';
				
			end if;		
		end if;
	end process;
	
	o_address <= chipaddress;
	o_map <= mapaddress;
	o_data <= data;
	o_start <= start;
end behavioral;