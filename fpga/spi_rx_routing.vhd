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
-- v0.0.7, 19.12.2025
-- OpenX32 Project
-- https://github.com/OpenMixerProject/OpenX32
--
-- This block received 16 bits from i.MX25:
-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
-- 0  |----- DATA -----| 0 |--- ADDR ---|
-- 

-- this version has an automatic address incrementer implemented to reduce amount of data on SPI-bus
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
	type t_SM is (s_Ready, s_Rx, s_Output, s_AutoIncrement, s_RxAi, s_OutputAi, s_Cleanup, s_WaitCsHigh);
	signal s_SM				: t_SM := s_Ready;
	
	signal address_ai		: std_logic_vector(6 downto 0) := (others => '0');
	signal z_spi_ncs		: std_logic;
	signal z_spi_clk		: std_logic;
	signal z_spi_data		: std_logic;
	signal zz_spi_clk		: std_logic;
	signal rx_data			: std_logic_vector(15 downto 0);
	signal rx_bit_count	: integer range 0 to 16 := 0;
begin
	process (clk)
	begin
		if rising_edge(clk) then
			-- set default-value for write-enable to low
			o_cfg_wr_en <= '0';

			z_spi_ncs <= i_spi_ncs;
			z_spi_data <= i_spi_data;
			z_spi_clk <= i_spi_clk;
			zz_spi_clk <= z_spi_clk;
		
			if (s_SM = s_Ready) then
				-- wait for ChipSelect to be asserted
				rx_bit_count <= 0;
				
				if (z_spi_ncs = '0') then
					s_SM <= s_Rx; -- we are now receiving
				end if;
				
			elsif (s_SM = s_Rx) then
				-- read data into shift-register on rising-edge
				if (z_spi_clk = '1' and zz_spi_clk = '0') then
					-- rising edge of SPI_CLK

					-- we are receiving:
					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
					-- 0  |----- DATA -----| 0 |--- ADDR ---|
					rx_data <= rx_data(14 downto 0) & z_spi_data;
					rx_bit_count <= rx_bit_count + 1;
				end if;

				if (rx_bit_count >= 16) then
					-- we just received the expected 16 bytes -> output received data to RAM-block
					s_SM <= s_Output;
				elsif (z_spi_ncs = '1') then
					-- ChipSelect deasserted -> abort
					s_SM <= s_Ready;
				end if;

			elsif (s_SM = s_Output) then
					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
					-- 0  |----- DATA -----| 0 |--- ADDR ---|
					o_cfg_wr_addr <= rx_data(6 downto 0); -- set 7 bit address
					o_cfg_wr_data <= rx_data(14 downto 8); -- set 7 bit output-source
					o_cfg_wr_en <= '1';

					if (rx_data(7) = '1') then
						-- auto-increment-bit is set and ChipSelect is still asserted
						address_ai <= rx_data(6 downto 0);
						s_SM <= s_AutoIncrement;
					else
						s_SM <= s_Cleanup;
					end if;

			elsif (s_SM = s_AutoIncrement) then
				o_cfg_wr_en <= '0';
				
				-- increment address
				address_ai <= std_logic_vector(unsigned(address_ai) + 1);
				rx_bit_count <= 0;
				
				s_SM <= s_RxAi;

			elsif (s_SM = s_RxAi) then
				-- read data into shift-register on rising-edge
				if (z_spi_clk = '1' and zz_spi_clk = '0') then
					-- rising edge of SPI_CLK

					-- in auto-increment-mode we are receiving only 8 bits
					-- the upper 8 bits of the shift-register are unused:
					-- 
					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
					--  x  x  x  x  x  x x x 0 |-- DATA ---|
					rx_data <= rx_data(14 downto 0) & z_spi_data;
					rx_bit_count <= rx_bit_count + 1;
				end if;

				if (rx_bit_count >= 8) then
					-- we received the expected 8 bytes when in auto-increment-mode -> output received data to RAM-block
					s_SM <= s_OutputAi;
				elsif (z_spi_ncs = '1') then
					-- ChipSelect deasserted -> abort
					s_SM <= s_Ready;
				end if;

			elsif (s_SM = s_OutputAi) then
					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
					-- 0  |----- DATA -----| 0 |--- ADDR ---|
					o_cfg_wr_addr <= address_ai; -- set 7 bit address
					o_cfg_wr_data <= rx_data(6 downto 0); -- set 7 bit output-source
					o_cfg_wr_en <= '1';

					s_SM <= s_AutoIncrement;

			elsif (s_SM = s_Cleanup) then
				o_cfg_wr_en <= '0';
				
				s_SM <= s_WaitCsHigh;
			
			elsif (s_SM = s_WaitCsHigh) then
				-- wait until ChipSelect is deasserted
				if (z_spi_ncs = '1') then
					s_SM <= s_Ready;
				end if;
				
			else
				s_SM <= s_Ready;
				
			end if;
		end if;
	end process;
end behavioral;

--library ieee;
--use ieee.std_logic_1164.all;
--use ieee.numeric_std.all;
--
--entity spi_rx_routing is
--	port (
--		clk				: in std_logic;
--		i_spi_ncs		: in std_logic;
--		i_spi_clk		: in std_logic;
--		i_spi_data		: in std_logic;
--		
--		o_cfg_wr_addr	: out std_logic_vector(6 downto 0);
--		o_cfg_wr_data	: out std_logic_vector(6 downto 0);
--		o_cfg_wr_en		: out std_logic
--	);
--end entity;
--
--architecture behavioral of spi_rx_routing is
--	type t_SM is (s_Ready, s_Rx, s_Output, s_Cleanup);
--	signal s_SM				: t_SM := s_Ready;
--	
--	signal z_spi_ncs		: std_logic;
--	signal z_spi_clk		: std_logic;
--	signal z_spi_data		: std_logic;
--	signal rx_data			: std_logic_vector(15 downto 0);
--	signal rx_bit_count	: integer range 0 to 20 := 0;
--begin
--	process (clk)
--	begin
--		if rising_edge(clk) then
--			z_spi_ncs <= i_spi_ncs;
--			z_spi_data <= i_spi_data;
--			z_spi_clk <= i_spi_clk;
--		
--			if (s_SM = s_Ready) then
--				-- wait for ChipSelect to be asserted
--				rx_bit_count <= 0;
--				
--				if (z_spi_ncs = '0') then
--					s_SM <= s_Rx; -- we are now receiving
--				end if;
--				
--			elsif (s_SM = s_Rx) then
--				-- read data into shift-register on rising-edge
--				if (i_spi_clk = '1' and z_spi_clk = '0') then
--					-- rising edge of SPI_CLK
--
--					-- we are receiving:
--					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
--					-- 0  |----- DATA -----| 0 |--- ADDR ---|
--					rx_data <= rx_data(14 downto 0) & z_spi_data;
--					rx_bit_count <= rx_bit_count + 1;
--				end if;
--
--				if ((rx_bit_count = 15) and (i_spi_clk = '1' and z_spi_clk = '0')) then
--					-- we just received the expected 16 bytes -> output received data to RAM-block
--					s_SM <= s_Output;
--				elsif (z_spi_ncs = '1' and rx_bit_count > 0) then
--					-- ChipSelect deasserted and we received less than 16 bytes -> abort
--					s_SM <= s_Ready;
--				end if;
--
--			elsif (s_SM = s_Output) then
--					-- 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
--					-- 0  |----- DATA -----| 0 |--- ADDR ---|
--					o_cfg_wr_addr <= rx_data(6 downto 0); -- set 7 bit address
--					o_cfg_wr_data <= rx_data(14 downto 8); -- set 7 bit output-source
--					o_cfg_wr_en <= '1';
--
--					s_SM <= s_Cleanup;
--
--			elsif (s_SM = s_Cleanup) then
--				o_cfg_wr_en <= '0';
--			
--				s_SM <= s_Ready;
--				
--			end if;
--		end if;
--	end process;
--end behavioral;
