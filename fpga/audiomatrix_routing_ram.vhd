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

entity audiomatrix_routing_ram is
    generic (
        RAM_DEPTH       : integer := 256; -- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40x DSP-Output, 48x AES50A, 48x AES50B
        ADDR_WIDTH      : integer := 8    -- log2(208) = 8
    );
    port (
        clk             : in std_logic;
		
        -- write-port for rs232-receiver
        cfg_wr_addr     : in std_logic_vector(ADDR_WIDTH - 1 downto 0);    -- output-channel to be configured
        cfg_wr_data     : in std_logic_vector(ADDR_WIDTH - 1 downto 0);    -- source for the current output-channel
        cfg_wr_en       : in std_logic;

		  -- read port for audio-matrix
        channel_idx		: in std_logic_vector(ADDR_WIDTH - 1 downto 0); -- current output-channel-index (0-based: 0 = ch1, 1=ch2, ...)
        read_addr    	: out std_logic_vector(ADDR_WIDTH - 1 downto 0)   -- source-channel, that will be copied to output
    );
end entity audiomatrix_routing_ram;

architecture behavioral of audiomatrix_routing_ram is
    type cfg_ram_type is array (RAM_DEPTH - 1 downto 0) of std_logic_vector(ADDR_WIDTH - 1 downto 0);
    signal cfg_ram_inst : cfg_ram_type := (others => (others => '0'));
begin
    process(clk)
    begin
        if rising_edge(clk) then
            -- 1. writing (configuration)
				if cfg_wr_en = '1' then
					cfg_ram_inst(to_integer(unsigned(cfg_wr_addr))) <= cfg_wr_data(ADDR_WIDTH - 1 downto 0);
				end if;
            
            -- 2. reading (routing-Lookup)
            read_addr <= cfg_ram_inst(to_integer(unsigned(channel_idx))); -- read the source-address for the current channel
				--read_addr <= std_logic_vector(to_unsigned(1, ADDR_WIDTH)); -- lock to channel 1 (XLR input 1)
				--read_addr <= std_logic_vector(unsigned(channel_idx(2 downto 0)) + 1); -- fixed routing to first 8 channels (XLR-inputs 1-8)
        end if;
    end process;
end architecture behavioral;
