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
		clk_rate_hz    : integer := 124000000;
		txd_rate_hz   : integer := 1
	);
	port
	(
		clk		: in std_logic;
		byte1		: in unsigned(7 downto 0); -- Version
		byte2		: in unsigned(7 downto 0); -- info about noisegates
		byte3		: in unsigned(7 downto 0); -- info about dynamic compressors
		byte4		: in unsigned(7 downto 0); -- info about clipping
		TX_rdy		: in std_logic := '0'; -- transmitter is ready for next byte
		
		TX_data	:	out unsigned(7 downto 0); -- databyte to transmit
		TX_send	:	out std_logic := '0' -- send data via transmitter
	);
end entity;

architecture Behavioral of rs232_encoder is
	constant PAYLOAD_TO_TX : integer := 4;
	
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
					bytearray(0) <= byte1;
					bytearray(1) <= byte2;
					bytearray(2) <= byte3;
					bytearray(3) <= byte4;

					-- calculate the checksum
					PayloadSum <= to_unsigned(0, 16);

					-- go into send-state
					s_SM_Encoder <= s_Send;
				when s_Send =>

					case byte_cnt is
						when 0 =>
							-- start of frame
							TX_data <= to_unsigned(42, 8); -- character "*"
						when PAYLOAD_TO_TX + 1 =>
							-- payload part 1
							TX_data <= PayloadSum(15 downto 8);
						when PAYLOAD_TO_TX + 2 =>
							-- payload part 2
							TX_data <= PayloadSum(7 downto 0);
						when PAYLOAD_TO_TX + 3 =>
							-- end of frame
							TX_data <= to_unsigned(35, 8); -- character "#"
						when others =>
							-- byte 1...end
							TX_data <= bytearray(byte_cnt - 1);
							
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
