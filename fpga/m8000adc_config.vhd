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
-- VHDL File to control Midas M8000 ADC IC
-- v0.0.1, 29.10.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity m8000adc_config is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_start		: in std_logic;
		i_txbusy		: in std_logic;
		
		o_address	: out std_logic_vector(7 downto 0);
		o_map			: out std_logic_vector(7 downto 0);
		o_data		: out std_logic_vector(7 downto 0);
		o_start		: out std_logic;
		o_done		: out std_logic
	);
end entity;

architecture behavioral of m8000adc_config is
	type t_SM is (s_Startup, s_Config, s_Wait, s_Done);
	signal s_SM				: t_SM := s_Startup;
	signal count_state	: natural range 0 to 50 := 0; -- allow up to 3us
	
	signal mapaddress		: std_logic_vector(7 downto 0);
	signal data				: std_logic_vector(7 downto 0);
	signal start			: std_logic;
	signal done				: std_logic;
	
	signal count_cfg		: natural range 0 to 3 := 0;
	constant chipaddress	: std_logic_vector(7 downto 0) := "10011110"; -- 7-bit address followed by R/nW
	type m8000adc_cfg_lut_t is array(0 to 2, 0 to 1) of std_logic_vector(7 downto 0);
	constant m8000adc_cfg_lut: m8000adc_cfg_lut_t := (
		(x"01", "00001011"), -- CLKMODE && MDIV [1:0] = Divide by 1 | DIF[1:0] = 10 = TDM | MODE[1:0] = 11 = SlaveMode
		(x"0A", "00001110"), -- Disable SDOUT2, 3, 4
		(x"01", "10001011") -- ControlPortMode | CLKMODE && MDIV [1:0] = Divide by 1 | DIF[1:0] = 10 = TDM | MODE[1:0] = 11 = SlaveMode
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
					done <= '0';
					count_cfg <= 0;
					s_SM <= s_Config;
				end if;

			elsif (s_SM = s_Config) then

				-- transmit bits over SPI-interface
				mapaddress <= m8000adc_cfg_lut(count_cfg, 0); -- set map address
				data <= m8000adc_cfg_lut(count_cfg, 1); -- set the data
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
						if (count_cfg = 2) then
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
				done <= '1';
				
			end if;		
		end if;
	end process;
	
	o_address <= chipaddress;
	o_map <= mapaddress;
	o_data <= data;
	o_start <= start;
	o_done <= done;
end behavioral;