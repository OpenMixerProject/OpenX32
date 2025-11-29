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
-- RS232 data encoder
-- (c) 2023-2025 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/OpenX32
--
-- This file contains a RS232 data-encoder to transmit individual bytes via a rs232-transmitter
-- It is like a signal multiplexer, that will prepare multiple bytes for a serial-transmitter

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity rs232_encoder is 
	generic(
		clk_rate_hz    : integer := 24576000;
		txd_rate_hz   : integer := 1
	);
	port
	(
		clk		: in std_logic;
		byte1		: in std_logic_vector(7 downto 0); -- o
		byte2		: in std_logic_vector(7 downto 0); -- X
		byte3		: in std_logic_vector(7 downto 0); -- 3
		byte4		: in std_logic_vector(7 downto 0); -- 2
		byte5		: in std_logic_vector(7 downto 0); -- v
		byte6		: in std_logic_vector(7 downto 0); -- x
		byte7		: in std_logic_vector(7 downto 0); -- .
		byte8		: in std_logic_vector(7 downto 0); -- x
		byte9		: in std_logic_vector(7 downto 0); -- x
		TX_rdy		: in std_logic := '0'; -- transmitter is ready for next byte
		
		TX_data	:	out std_logic_vector(7 downto 0); -- databyte to transmit
		TX_send	:	out std_logic := '0' -- send data via transmitter
	);
end entity;

architecture Behavioral of rs232_encoder is
	constant PAYLOAD_TO_TX : integer := 9;
	
	type t_SM_Encoder is (s_Idle, s_Send, s_Prepare, s_Wait);
	signal s_SM_Encoder	: t_SM_Encoder := s_Idle;
	
	signal txd_rate_cnt	: natural range 0 to clk_rate_hz/(2*txd_rate_hz) := 0;
	signal byte_cnt		: natural range 0 to PAYLOAD_TO_TX + 4 - 1 := 0; -- payload-bytes + 4 frame-bytes
	
	type t_bytearray is array(PAYLOAD_TO_TX - 1 downto 0) of unsigned(7 downto 0);
	signal bytearray	: t_bytearray;
	signal PayloadSum	: unsigned(15 downto 0);
begin
	process(clk)
	begin
		if (rising_edge(clk)) then
			case s_SM_Encoder is
				when s_Idle =>
					-- waiting for next send-interval
					TX_send <= '0';
					
					txd_rate_cnt <= txd_rate_cnt + 1;
					if (txd_rate_cnt = ((clk_rate_hz/(2*txd_rate_hz) - 1))) then -- 4MHz / 200000 = 20 Hz, but we are inverting on each clock-edge, so 10 Hz
						-- we reached the user-defined send-interval -> start new frame
						s_SM_Encoder <= s_Prepare;

						-- reset counter for next interval
						txd_rate_cnt <= 0;
					end if;

				when s_Prepare =>
					-- reset byte-counter
					byte_cnt <= 0;
				
					-- copy all bytes to internal signals as data will change during the transmission
					bytearray(0) <= unsigned(byte1);
					bytearray(1) <= unsigned(byte2);
					bytearray(2) <= unsigned(byte3);
					bytearray(3) <= unsigned(byte4);
					bytearray(4) <= unsigned(byte5);
					bytearray(5) <= unsigned(byte6);
					bytearray(6) <= unsigned(byte7);
					bytearray(7) <= unsigned(byte8);
					bytearray(8) <= unsigned(byte9);

					-- calculate the checksum
					PayloadSum <= to_unsigned(0, 16);

					-- go into send-state
					s_SM_Encoder <= s_Send;
				when s_Send =>

					case byte_cnt is
						when 0 =>
							-- start of frame
							TX_data <= std_logic_vector(to_unsigned(42, 8)); -- character "*"
						when PAYLOAD_TO_TX + 1 =>
							-- payload part 1
							TX_data <= std_logic_vector(PayloadSum(15 downto 8));
						when PAYLOAD_TO_TX + 2 =>
							-- payload part 2
							TX_data <= std_logic_vector(PayloadSum(7 downto 0));
						when PAYLOAD_TO_TX + 3 =>
							-- end of frame
							TX_data <= std_logic_vector(to_unsigned(35, 8)); -- character "#"
						when others =>
							-- byte 1...end
							TX_data <= std_logic_vector(bytearray(byte_cnt - 1));
							
							PayloadSum <= PayloadSum + resize(bytearray(byte_cnt - 1), 16);
					end case;

					-- clear to send
					TX_send <= '1';

					if (byte_cnt < (PAYLOAD_TO_TX + 3)) then
						-- increase byte-counter
						byte_cnt <= byte_cnt + 1;
						
						-- go into wait-state and wait for transmitter to transmit next byte
						s_SM_Encoder <= s_Wait;
					else
						-- all data transmitted -> go into idle-state
						s_SM_Encoder <= s_Idle;
					end if;
					
				when s_Wait =>
					-- wait for transmitter

					-- reset clear-flag
					TX_send <= '0';
					
					-- check if we are ready for next byte
					if (TX_rdy = '1') then
						s_SM_Encoder <= s_Send;
					end if;
					
			end case;
		end if;
	end process;
end Behavioral;
