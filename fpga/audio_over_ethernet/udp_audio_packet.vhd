-- UDP Audio Packet Sender
-- (c) 2025 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/AES50_Transmitter
--
-- This file contains an ethernet-packet-generator to send individual bytes to an EthernetMAC directly.

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity udp_audio_packet is
	port
	(
		src_mac_address		: in std_logic_vector(47 downto 0);
		src_ip_address			: in std_logic_vector(31 downto 0);
		dst_mac_address		: in std_logic_vector(47 downto 0);
		dst_ip_address			: in std_logic_vector(31 downto 0);
		src_udp_port			: in std_logic_vector(15 downto 0);
		dst_udp_port			: in std_logic_vector(15 downto 0);
		tx_clk					: in std_logic;
		tx_busy					: in std_logic;
		tx_byte_sent			: in std_logic;
		audio_ch0_in			: in std_logic_vector(23 downto 0);
		audio_ch1_in			: in std_logic_vector(23 downto 0);
		audio_ch2_in			: in std_logic_vector(23 downto 0);
		audio_ch3_in			: in std_logic_vector(23 downto 0);
		audio_ch4_in			: in std_logic_vector(23 downto 0);
		audio_ch5_in			: in std_logic_vector(23 downto 0);
		audio_ch6_in			: in std_logic_vector(23 downto 0);
		audio_ch7_in			: in std_logic_vector(23 downto 0);
		audio_ch8_in			: in std_logic_vector(23 downto 0);
		audio_ch9_in			: in std_logic_vector(23 downto 0);
		audio_ch10_in			: in std_logic_vector(23 downto 0);
		audio_ch11_in			: in std_logic_vector(23 downto 0);
		audio_ch12_in			: in std_logic_vector(23 downto 0);
		audio_ch13_in			: in std_logic_vector(23 downto 0);
		audio_ch14_in			: in std_logic_vector(23 downto 0);
		audio_ch15_in			: in std_logic_vector(23 downto 0);
		audio_sync				: in std_logic;

		tx_enable				: out std_logic := '0';  -- TX valid
		tx_data					: out std_logic_vector(7 downto 0) := (others => '0') -- data-octet
	);
end entity;

architecture Behavioral of udp_audio_packet is
	-- some general thoughts:
	-- with a maximum payload of 1460bytes (= 365 4-byte-samples), we could use 32 channels with a buffer of 11 samples
	-- when sending only 3 bytes per channel, we have a payload of 486 samples, which means we can transmit all 48 channels with a buffer of 10 samples

	-- Constants
	constant BUFFERED_AUDIO_SAMPLES	: integer := 16; -- keep limit of 1460 bytes per UDP-frame in mind
	constant AUDIO_CHANNELS				: integer := 16; -- keep audio-channels factor of two if using 24-bit per sample. Otherwise we will get uneven byte-numbers due to 3-byte-samples
	constant BYTES_PER_SAMPLE			: integer := 3; -- 2 (16-bit), 3 (24-bit) or 4 (32-bit)
	constant AUDIO_START_SIGNAL		: integer := 8;
	constant AUDIO_BUFFER_LENGTH		: integer := BUFFERED_AUDIO_SAMPLES * AUDIO_CHANNELS * BYTES_PER_SAMPLE;
	
	constant MAC_HEADER_LENGTH			: integer := 14;
	constant IP_HEADER_LENGTH			: integer := 5 * (32 / 8); -- Header length always 20 bytes (5 * 32 bit words)
	constant UDP_PSEUDO_HEADER_LENGTH: integer := 8;
	constant UDP_HEADER_LENGTH			: integer := 8;
	constant UDP_PAYLOAD_LENGTH		: integer := AUDIO_START_SIGNAL + AUDIO_BUFFER_LENGTH; -- 8 start-bytes + x bytes for audio
	constant PACKET_LENGTH				: integer := MAC_HEADER_LENGTH + IP_HEADER_LENGTH + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH;

	-- Checksum calculation
	signal checksum						: unsigned(15 downto 0) := (others => '0');
	signal checksum_tmp					: unsigned(31 downto 0) := (others => '0');
	signal checksum_byte_count			: integer range 0 to IP_HEADER_LENGTH + 2;
	signal calculating_checksum		: std_logic := '0';
	signal calc_new_checksum			: std_logic := '0';

	signal udp_checksum					: unsigned(15 downto 0) := (others => '0');
	signal udp_checksum_tmp				: unsigned(31 downto 0) := (others => '0');
	signal udp_checksum_byte_count	: integer range 0 to UDP_PSEUDO_HEADER_LENGTH + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH + 20; -- keep enough headroom
	signal udp_calculating_checksum	: std_logic := '0';
	signal udp_calc_new_checksum		: std_logic := '0';

	-- Other signals used in this file
	type t_SM_Ethernet is (s_Idle, s_CalcChecksum, s_WaitChecksum, s_Start, s_Wait, s_Transmit, s_End);
	signal s_SM_Ethernet					: t_SM_Ethernet := s_Idle;
	signal byte_counter					: integer range 0 to 1600 := 0; -- one ethernet-frame cannot take more than 1500 bytes + header
	signal packet_counter				: integer range 0 to 65535 := 1;

	type t_ethernet_frame is array (0 to PACKET_LENGTH - 1) of std_logic_vector(7 downto 0);
	signal udp_frame					: t_ethernet_frame;
	type t_sample_buffer is array (0 to AUDIO_BUFFER_LENGTH - 1) of std_logic_vector(7 downto 0);
	signal sample_buffer				: t_sample_buffer;
	
	signal frame_start : std_logic := '0';
	signal zaudio_sync : std_logic := '0';
	signal audio_buffer_ptr : integer range 0 to PACKET_LENGTH + 20 := 0; -- keep some headroom
begin         
	process (tx_clk)
		variable Word: std_logic_vector(15 downto 0);
		variable udpWord: std_logic_vector(15 downto 0); -- process multiple words at once
	begin
		if (falling_edge(tx_clk)) then
			zaudio_sync <= audio_sync;
			-- copy audio-data to buffer-array when we are receiving new samples (audio_sync)
			if ((audio_sync = '1') and (zaudio_sync = '0')) then
				-- rising edge of audio_sync -> read audio-data
				if (audio_buffer_ptr < (AUDIO_BUFFER_LENGTH - (2 * AUDIO_CHANNELS * BYTES_PER_SAMPLE))) then
					-- increment buffer-pointer by x bytes (6 bytes for stereo 24bit)
					audio_buffer_ptr <= audio_buffer_ptr + (AUDIO_CHANNELS * BYTES_PER_SAMPLE); -- we are storing AUDIO_CHANNELS * x bytes
				elsif (audio_buffer_ptr = (AUDIO_BUFFER_LENGTH - (2 * AUDIO_CHANNELS * BYTES_PER_SAMPLE))) then
					-- buffer-pointer has reached the last element
					audio_buffer_ptr <= audio_buffer_ptr + (AUDIO_CHANNELS * BYTES_PER_SAMPLE); -- we are storing AUDIO_CHANNELS * x bytes
				else
					-- next buffer-pointer would be out of scope, so reset to first element
					frame_start <= '1'; -- set flag to read buffer when state-machine enteres s_Idle again
					audio_buffer_ptr <= 0; -- reset to first element
				end if;
				
				sample_buffer(audio_buffer_ptr + 0) <= audio_ch0_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 1) <= audio_ch0_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 2) <= audio_ch0_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 3) <= audio_ch1_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 4) <= audio_ch1_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 5) <= audio_ch1_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 6) <= audio_ch2_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 7) <= audio_ch2_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 8) <= audio_ch2_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 9) <= audio_ch3_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 10) <= audio_ch3_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 11) <= audio_ch3_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 12) <= audio_ch4_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 13) <= audio_ch4_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 14) <= audio_ch4_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 15) <= audio_ch5_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 16) <= audio_ch5_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 17) <= audio_ch5_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 18) <= audio_ch6_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 19) <= audio_ch6_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 20) <= audio_ch6_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 21) <= audio_ch7_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 22) <= audio_ch7_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 23) <= audio_ch7_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 24) <= audio_ch8_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 25) <= audio_ch8_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 26) <= audio_ch8_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 27) <= audio_ch9_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 28) <= audio_ch9_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 29) <= audio_ch9_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 30) <= audio_ch10_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 31) <= audio_ch10_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 32) <= audio_ch10_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 33) <= audio_ch11_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 34) <= audio_ch11_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 35) <= audio_ch11_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 36) <= audio_ch12_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 37) <= audio_ch12_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 38) <= audio_ch12_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 39) <= audio_ch13_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 40) <= audio_ch13_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 41) <= audio_ch13_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 42) <= audio_ch14_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 43) <= audio_ch14_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 44) <= audio_ch14_in(23 downto 16); -- MSB of audiosample

				sample_buffer(audio_buffer_ptr + 45) <= audio_ch15_in(7 downto 0); -- LSB of audiosample
				sample_buffer(audio_buffer_ptr + 46) <= audio_ch15_in(15 downto 8);
				sample_buffer(audio_buffer_ptr + 47) <= audio_ch15_in(23 downto 16); -- MSB of audiosample
			end if;
		
			-- send UDP-frames with stored audio-data
			if ((frame_start = '1') and (s_SM_Ethernet = s_Idle)) then
				frame_start <= '0';
				-- prepare begin of packet
				packet_counter <= packet_counter + 1; -- increment packet counter
				tx_enable <= '0';
				byte_counter <= 0;
				
				-- 7 preamble bytes + SFD will be added by Ethernet-MAC
				
				-- MAC HEADER (14 bytes)
				-- fill MAC-Header with desired values 00-24-9B-7F-B9-2A
				udp_frame(0) <= dst_mac_address(47 downto 40); -- MSB contains typical left side of MAC
				udp_frame(1) <= dst_mac_address(39 downto 32);
				udp_frame(2) <= dst_mac_address(31 downto 24);
				udp_frame(3) <= dst_mac_address(23 downto 16);
				udp_frame(4) <= dst_mac_address(15 downto 8);
				udp_frame(5) <= dst_mac_address(7 downto 0);

				udp_frame(6) <= src_mac_address(47 downto 40); -- MSB contains typical left side of MAC
				udp_frame(7) <= src_mac_address(39 downto 32);
				udp_frame(8) <= src_mac_address(31 downto 24);
				udp_frame(9) <= src_mac_address(23 downto 16);
				udp_frame(10) <= src_mac_address(15 downto 8);
				udp_frame(11) <= src_mac_address(7 downto 0);

				-- IP Protocol
				udp_frame(12) <= x"08"; -- type [0x0800 = IP Protocol]
				udp_frame(13) <= x"00";

				-- IP HEADER (20 bytes)
				udp_frame(14) <= x"45"; -- b14 = version (4-bit) | internet header length (4-bit) [Version 4 and header length of 0x05 = 20 bytes]
				udp_frame(15) <= x"00"; -- differentiated services (6-bits) | explicit congestion notification (2-bits)
				udp_frame(16) <= std_logic_vector(to_unsigned(IP_HEADER_LENGTH + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH, 16)(15 downto 8)); -- total length without MAC-header: entire packet size in bytes, including IP-header and payload-data. The minimum size is 46 bytes of user data (= 0x2e, header without data) and the maximum is 65,535 bytes
				udp_frame(17) <= std_logic_vector(to_unsigned(IP_HEADER_LENGTH + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH, 16)(7 downto 0)); -- 20 bytes IP-header + 8 bytes UDP-header + 18 bytes UDP-payload = 46 bytes = 0x002e
				udp_frame(18) <= std_logic_vector(to_unsigned(packet_counter, 16))(15 downto 8); -- identification (primarily used for uniquely identifying the group of fragments of a single IP datagram) [0x0000 will be ignored by windows, so we set the packet_counter to this value in the next step]
				udp_frame(19) <= std_logic_vector(to_unsigned(packet_counter, 16))(7 downto 0);
				udp_frame(20) <= x"00"; -- flags (3-bits) | fragment offsets (13-bits)
				udp_frame(21) <= x"00";
				udp_frame(22) <= x"80"; -- time to live (0x80 = 128)
				udp_frame(23) <= x"11"; -- b23 = protocol (0x01 = ICMP, 0x06 = TCP, 0x11 = UDP)
				udp_frame(24) <= x"00"; -- header checksum (16-bit ones' complement of the ones' complement sum of all 16-bit words in the header)
				udp_frame(25) <= x"00";

				udp_frame(26) <= src_ip_address(31 downto 24); -- MSB contains typical "192"
				udp_frame(27) <= src_ip_address(23 downto 16);
				udp_frame(28) <= src_ip_address(15 downto 8);
				udp_frame(29) <= src_ip_address(7 downto 0);
				
				udp_frame(30) <= dst_ip_address(31 downto 24); -- MSB contains typical "192"
				udp_frame(31) <= dst_ip_address(23 downto 16);
				udp_frame(32) <= dst_ip_address(15 downto 8);
				udp_frame(33) <= dst_ip_address(7 downto 0);
				-- options | padding
				
				-- UDP HEADER (8 bytes)
				udp_frame(34) <= src_udp_port(15 downto 8);
				udp_frame(35) <= src_udp_port(7 downto 0);
				udp_frame(36) <= dst_udp_port(15 downto 8);
				udp_frame(37) <= dst_udp_port(7 downto 0);
				udp_frame(38) <= std_logic_vector(to_unsigned(UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH, 16)(15 downto 8)); -- length (length of this UDP packet including header and data. Minimum 8 bytes)
				udp_frame(39) <= std_logic_vector(to_unsigned(UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH, 16)(7 downto 0));
				udp_frame(40) <= x"00"; -- checksum (0 is a valid CRC-value to ignore it)
				udp_frame(41) <= x"00";

				-- UDP PAYLOAD (8 + audiodata bytes)
				-- payload is set above
				udp_frame(42) <= x"4E"; -- N
				udp_frame(43) <= x"44"; -- D
				udp_frame(44) <= x"4E"; -- N
				udp_frame(45) <= x"47"; -- G
				udp_frame(46) <= std_logic_vector(to_unsigned(packet_counter, 16))(15 downto 8);--x"f0";
				udp_frame(47) <= std_logic_vector(to_unsigned(packet_counter, 16))(7 downto 0);--x"f0";
				udp_frame(48) <= "01" & std_logic_vector(to_unsigned(AUDIO_CHANNELS, 6)); -- bits 7..6 = samplerate (00 = 44.1kHz, 01 = 48kHz, 10=96kHz, 11=192kHz) | bits 5..0 = channel count (0...63)
				udp_frame(49) <= std_logic_vector(to_unsigned(BUFFERED_AUDIO_SAMPLES, 8)); -- samples per packet
				for i in 0 to AUDIO_BUFFER_LENGTH - 1 loop
					udp_frame(50 + i) <= sample_buffer(i); -- copy content of audio-buffer
				end loop;

				checksum_tmp                <= (others => '0');
				checksum_byte_count         <= 0;
				calculating_checksum        <= '1';

				udp_checksum_tmp            <= to_unsigned(17 + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH, 32); -- 0x11 (protocol) + UDP-LENGTH (header+payload)
				udp_checksum_byte_count     <= 0;
				udp_calculating_checksum    <= '1';

				s_SM_Ethernet <= s_CalcChecksum;
				
			elsif (s_SM_Ethernet = s_CalcChecksum) then
				-- calculate checksum for IP-Header
				if (checksum_byte_count < IP_HEADER_LENGTH) then
					Word                    := udp_frame(MAC_HEADER_LENGTH + checksum_byte_count) & udp_frame(MAC_HEADER_LENGTH + checksum_byte_count + 1);
					checksum_tmp            <= checksum_tmp + resize(unsigned(Word), 32);
					checksum_byte_count     <= checksum_byte_count + 2; -- we are reading two bytes at once
				else
					-- checksum is calculated -> make sure that we have only 2-byte checksum and add carryover above 16th bit to 16-bit checksum
					if (checksum_tmp(31 downto 16) > 0) then
						checksum_tmp <= resize(checksum_tmp(15 downto 0), 32) + resize(checksum_tmp(31 downto 16), 32);
					else
						checksum                <= x"ffff" - checksum_tmp(15 downto 0);
						calculating_checksum    <= '0';
					end if;
				end if;
			
				-- calculate checksum for UDP-Payload
				if (udp_checksum_byte_count < (UDP_PSEUDO_HEADER_LENGTH + UDP_HEADER_LENGTH + UDP_PAYLOAD_LENGTH)) then
					udpWord                    := udp_frame(MAC_HEADER_LENGTH + IP_HEADER_LENGTH - UDP_PSEUDO_HEADER_LENGTH + udp_checksum_byte_count) & udp_frame(MAC_HEADER_LENGTH + IP_HEADER_LENGTH - UDP_PSEUDO_HEADER_LENGTH + udp_checksum_byte_count + 1);
					udp_checksum_tmp            <= udp_checksum_tmp + resize(unsigned(udpWord), 32);
					udp_checksum_byte_count     <= udp_checksum_byte_count + 2; -- we are reading 3*2 bytes at once
				else
					-- checksum is calculated -> make sure that we have only 2-byte checksum and add carryover above 16th bit to 16-bit checksum
					if (udp_checksum_tmp(31 downto 16) > 0) then
						udp_checksum_tmp <= resize(udp_checksum_tmp(15 downto 0), 32) + resize(udp_checksum_tmp(31 downto 16), 32);
					else
						-- calc inversion and stop checksum-calculation
						udp_checksum                <= x"ffff" - udp_checksum_tmp(15 downto 0);
						udp_calculating_checksum    <= '0';
					end if;
				end if;

				-- if both checksum are ready, go to next state
				if ((calculating_checksum = '0') and (udp_calculating_checksum = '0')) then
					s_SM_Ethernet <= s_Start;
				end if;
				
			elsif (s_SM_Ethernet = s_Start) then
				-- wait until MAC is ready again
				if (tx_busy = '0') then
					udp_frame(MAC_HEADER_LENGTH + 10) <= std_logic_vector(checksum(15 downto 8)); -- MSB
					udp_frame(MAC_HEADER_LENGTH + 11) <= std_logic_vector(checksum(7 downto 0)); -- LSB
					udp_frame(MAC_HEADER_LENGTH + IP_HEADER_LENGTH + 6) <= std_logic_vector(udp_checksum(15 downto 8)); -- MSB
					udp_frame(MAC_HEADER_LENGTH + IP_HEADER_LENGTH + 7) <= std_logic_vector(udp_checksum(7 downto 0)); -- LSB

					tx_enable <= '1';
					byte_counter <= 0; -- preload to first byte again
					tx_data <= udp_frame(0);

					s_SM_Ethernet <= s_Transmit;
				end if;
			
			elsif (s_SM_Ethernet = s_Transmit) then
				-- wait until previous byte is sent
				if (tx_byte_sent = '1') then
					-- send next byte and increment byte_counter
					tx_data <= udp_frame(byte_counter);
					
					if (byte_counter = PACKET_LENGTH - 1) then
						-- stop transmitting
						s_SM_Ethernet <= s_End;
					end if;
					
					byte_counter <= byte_counter + 1;
				end if;
				
			elsif (s_SM_Ethernet = s_End) then
				tx_enable <= '0';
				tx_data <= "00000000";

				s_SM_Ethernet <= s_Idle;
			end if;
		end if;
	end process;
end Behavioral;
