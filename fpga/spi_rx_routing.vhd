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
-- VHDL File to read from SPI-device
-- v0.0.1, 14.12.2025
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
--
-- This block received 16 bits from i.MX25:
-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
-- 0  |----- DATA -----| 0 |--- ADDR ---|
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_rx_routing is
	port (
		clk				: in std_logic;
		i_spi_ncs		: in std_logic;
		i_spi_clk		: in std_logic;
		i_spi_data		: in std_logic;
		
		o_cfg_wr_addr	: out std_logic_vector(6 downto 0);
		o_cfg_wr_data	: out std_logic_vector(6 downto 0);
		o_cfg_wr_en		: out std_logic
	);
end entity;

architecture behavioral of spi_rx_routing is
	type t_SM is (s_Ready, s_Rx, s_Cleanup);
	signal s_SM			: t_SM := s_Ready;
	
	signal z_spiclk	: std_logic;
	signal rx_data		: std_logic_vector(15 downto 0);
begin
	process (clk)
	begin
		if rising_edge(clk) then
			z_spiclk <= i_spi_clk;
		
			if (s_SM = s_Ready) then
				-- wait for ChipSelect to be asserted
				
				if (i_spi_ncs = '0') then
					s_SM <= s_Rx; -- we are now receiving
				end if;
				
			elsif (s_SM = s_Rx) then
				if (i_spi_ncs = '0') then
					-- chip select is (still) asserted
				
					-- read data into shift-register on rising-edge
					if (i_spi_clk = '1' and z_spiclk = '0') then
						-- rising edge of SPI_CLK

						-- we are receiving:
						-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
						-- 0  |----- DATA -----| 0 |--- ADDR ---|
						rx_data <= rx_data(14 downto 0) & i_spi_data;
					end if;
				else
					-- chip-select has been deasserted -> output received data to RAM-block
					
					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
					-- 0  |----- DATA -----| 0 |--- ADDR ---|
					o_cfg_wr_addr <= rx_data(6 downto 0); -- set 7 bit address
					o_cfg_wr_data <= rx_data(14 downto 8); -- set 7 bit output-source
					o_cfg_wr_en <= '1';
					
					s_SM <= s_Cleanup;
				end if;
			elsif (s_SM = s_Cleanup) then
				o_cfg_wr_en <= '0';
			
				s_SM <= s_Ready;
				
			end if;
		end if;
	end process;
end behavioral;