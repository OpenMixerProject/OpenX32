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
-- VHDL File to store routing-information in BRAM for audio-matrix
-- v0.0.1, 30.11.2025
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity debug_data is
    port (
        clk             : in std_logic;
		
        -- write-port for rs232-receiver
        cfg_wr_addr     : in std_logic_vector(7 downto 0);
        cfg_wr_data     : in std_logic_vector(7 downto 0);
        cfg_wr_en       : in std_logic;

        debugbits	 		: out std_logic_vector(7 downto 0)
    );
end entity debug_data;

architecture behavioral of debug_data is
begin
    process(clk)
    begin
        if rising_edge(clk) then
				if ((cfg_wr_addr = "11111111") and (cfg_wr_en = '1')) then
					 debugbits <= cfg_wr_data(7 downto 0);
				end if;
        end if;
    end process;
end architecture behavioral;
