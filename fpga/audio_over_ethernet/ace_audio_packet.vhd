-- Allen and Heath ACE Audio-Packet-Generator
-- (c) 2026 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/OpenMixerProject/OpenX32
-- 
-- This file contains an ethernet-packet-generator to send individual bytes to an EthernetMAC directly
-- Part of this work is based on reverse-engineering of PatrLind: https://github.com/PatrLind/ah_ace_protocol
-- 
-- 
-- The ACE-protocol is quite simple:
-- ====================================================================================
-- 6 Bytes Destination-MAC-Address (FF:FF:FF:FF:FF:FF)
-- 6 Bytes Source-MAC-Address (00:04:C4:01:xx:xx)
-- 2 Bytes Length of primary payload ()
-- 4 Bytes 802.1Q Tag (0x00 0x02 0x00 0xDD)
-- 195 Bytes of PCM Audio-Data (65 Audio-Channels with 3 bytes per channel)
-- 26 Bytes of Control-Data (presumably ethernet)
-- 
-- Total data-size is always 221 bytes
-- The total frame size is either 239 bytes when using VLAN or 235 bytes without VLAN
-- 
-- 
-- 
-- ----------------------------------------------------------------------------------------------------
-- Begin of ACE-message-frame
-- ----------------------------------------------------------------------------------------------------
-- FFFFFFFFFFFF                                                // Destination MAC-Address
-- 0004C4010203                                                // Source MAC-Address
-- 00DD                                                        // Length of next payload-section (0x00DD = 221 Bytes)
-- ----------------------------------------------------------------------------------------------------
-- Begin of payload-section with 221 Bytes
-- ----------------------------------------------------------------------------------------------------
-- 000044                                                      // 3 bytes Sync-Channel (see below)
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 000000 000000 000000 000000 000000 000000 000000 000000     // 8x 3 bytes Audio-Data
-- 0000000000000000000000000000000000000000000000000000        // 26 bytes of Control-Data
-- ----------------------------------------------------------------------------------------------------
-- End of payload-section with 221 Bytes
-- ----------------------------------------------------------------------------------------------------
-- 000c010000060000                                            // 8 bytes of Header and Synchronisation / State of Control-Channel
-- 000c010000000000                                            // 8 bytes of Header and Synchronisation / State of Control-Channel
-- 006832bb1867fce743                                          // 9 bytes of payload-data of control-channel
-- eb000000                                                    // 4 bytes with total message length (0xEB = 235 bytes)
-- eb000000                                                    // 4 bytes with total message length (0xEB = 235 bytes)
-- ----------------------------------------------------------------------------------------------------
-- End of ACE-message-frame
-- ----------------------------------------------------------------------------------------------------
-- 
-- 
-- Audio-framerate is 48kHz with 24bit PCM-encoded audio
-- Channel 0 is the sync-channel with the following repeating content:
-- byte 0 (LSB): 6-bit counter: 0x40, 0x44, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C
-- byte 1..2 (MSB): 0x00
-- 
-- the type Control-Data is defined by the first byte:
-- 0x00 -> No control Data. All 25 bytes of Control Data are zero
-- 0x59 -> Start of new ethernet-frame
-- 0x99 -> 25 following bytes are valid
-- 0x80 -> 25 following bytes are invalid
-- 0x85 only the following 5 bytes are valid
-- 
-- Hence, control data contains chunks of regular ethernet frames with regular 8-byte preamble
-- To decode it, the 25 bytes have to be concatenated while dropping the trailing zeros at the end according
-- to Control-Data byte.

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity ace_audio_packet is
	port
	(
		src_mac_address		: in std_logic_vector(47 downto 0);
		--dst_mac_address		: in std_logic_vector(47 downto 0); -- at the moment we are broadcasting to FF:FF:FF:FF:FF:FF
		tx_clk					: in std_logic;
		tx_busy					: in std_logic;
		tx_byte_sent			: in std_logic;
		audio_in					: in std_logic_vector(1559 downto 0); -- 65 audio-channels with each 24-bit = 1.560 bits
		audio_sync				: in std_logic;

		tx_enable				: out std_logic := '0';  -- TX valid
		tx_data					: out std_logic_vector(7 downto 0) := (others => '0') -- data-octet
	);
end entity;

architecture Behavioral of ace_audio_packet is
	-- Constants
	constant AUDIO_CHANNELS				: integer := 1 + 64; -- 1 Sync-Channel + 64 Audio-Channels
	constant BYTES_PER_SAMPLE			: integer := 3;
	
	constant HEADER_LENGTH				: integer := 14; -- dest-/src-MAC-address + length of first payload-block
	constant AUDIO_BUFFER_LENGTH		: integer := AUDIO_CHANNELS * BYTES_PER_SAMPLE;
	constant CONTROL_DATA_LENGTH		: integer := 26; -- data for tunnelled network / control-signals
	constant TRAILING_DATA_LENGTH		: integer := 16 + 9 + 8; -- synchronization, control-channel, padding-bytes
	constant PACKET_LENGTH				: integer := HEADER_LENGTH + AUDIO_BUFFER_LENGTH + CONTROL_DATA_LENGTH + TRAILING_DATA_LENGTH;

	-- Other signals used in this file
	type t_SM_Ethernet is (s_Idle, s_CalcChecksum, s_WaitChecksum, s_Start, s_Wait, s_Transmit, s_End);
	signal s_SM_Ethernet					: t_SM_Ethernet := s_Idle;
	signal byte_counter					: integer range 0 to 300 := 0; -- one ACE-frame seem to have a maximum of 268 bytes. So limit to 300 bytes here
	--signal packet_counter				: integer range 0 to 65535 := 1; -- not used at the moment

	type t_ethernet_frame is array (0 to PACKET_LENGTH - 1) of std_logic_vector(7 downto 0);
	signal ace_frame		: t_ethernet_frame;
	type t_sample_buffer is array (0 to AUDIO_BUFFER_LENGTH - 1) of std_logic_vector(7 downto 0);
	signal sample_buffer		: t_sample_buffer;
	
	signal frame_start : std_logic := '0';
	signal zaudio_sync : std_logic := '0';
begin         
	process (tx_clk)
		variable bitPointer: integer range 0 to 1700; -- maximum is 1560 bits, but we give some headroom
	begin
		if (falling_edge(tx_clk)) then
			zaudio_sync <= audio_sync;
			-- pack a new ethernet-frame every 48kHz
			if ((audio_sync = '1') and (zaudio_sync = '0')) then
				frame_start <= '1'; -- set flag to read buffer when state-machine enteres s_Idle again
			end if;
		
			-- send UDP-frames with stored audio-data
			if ((frame_start = '1') and (s_SM_Ethernet = s_Idle)) then
				frame_start <= '0';
				
				-- prepare begin of packet
				--packet_counter <= packet_counter + 1; -- increment packet counter
				tx_enable <= '0';
				byte_counter <= 0;
				
				-- 7 preamble bytes + SFD will be added by Ethernet-MAC
				
				-- MAC HEADER (14 bytes)
				-- fill MAC-Header with desired values
				ace_frame(0) <= x"FF"; --dst_mac_address(47 downto 40); -- MSB contains typical left side of MAC
				ace_frame(1) <= x"FF"; --dst_mac_address(39 downto 32);
				ace_frame(2) <= x"FF"; --dst_mac_address(31 downto 24);
				ace_frame(3) <= x"FF"; --dst_mac_address(23 downto 16);
				ace_frame(4) <= x"FF"; --dst_mac_address(15 downto 8);
				ace_frame(5) <= x"FF"; --dst_mac_address(7 downto 0);

				ace_frame(6) <= src_mac_address(47 downto 40); -- MSB contains typical left side of MAC
				ace_frame(7) <= src_mac_address(39 downto 32);
				ace_frame(8) <= src_mac_address(31 downto 24);
				ace_frame(9) <= src_mac_address(23 downto 16);
				ace_frame(10) <= src_mac_address(15 downto 8);
				ace_frame(11) <= src_mac_address(7 downto 0);

				-- length of payload-data of first block (audio and control-data)
				ace_frame(12) <= x"00";
				ace_frame(13) <= x"DD"; -- 0x00DD = 221 bytes
				
				-- Audio-Data (195 bytes)
				-- byte 0 contains PCM byte 2
				-- byte 1 contains PCM byte 1
				-- byte 2 contains PCM byte 0
				-- nibbles of all bytes are swapped
				for i in 0 to AUDIO_CHANNELS - 1 loop
					bitPointer := i * 3 * 8;
				
					-- byte 0                             LOW-NIBBLE of byte 2          |        HIGH-NIBBLE of byte 2
					ace_frame(14 + (i * 3) + 0) <= audio_in(bitPointer + 3 + 16 downto bitPointer + 0 + 16) & audio_in(bitPointer + 7 + 16 downto bitPointer + 4 + 16);
					
					-- byte 1                             LOW-NIBBLE of byte 1          |        HIGH-NIBBLE of byte 1
					ace_frame(14 + (i * 3) + 1) <= audio_in(bitPointer + 3 + 8 downto bitPointer + 0 + 8) & audio_in(bitPointer + 7 + 8 downto bitPointer + 4 + 8);
					
					-- byte 2                             LOW-NIBBLE of byte 0          |        HIGH-NIBBLE of byte 0
					ace_frame(14 + (i * 3) + 2) <= audio_in(bitPointer + 3 + 0 downto bitPointer + 0 + 0) & audio_in(bitPointer + 7 + 0 downto bitPointer + 4 + 0);
				end loop;
				
				-- Control-Data (26 bytes)
				ace_frame(209) <= x"00"; -- control-value. If 0x00, then all control-data-bytes are 0x00
				ace_frame(210) <= x"00";
				ace_frame(211) <= x"00";
				ace_frame(212) <= x"00";
				ace_frame(213) <= x"00";
				ace_frame(214) <= x"00";
				ace_frame(215) <= x"00";
				ace_frame(216) <= x"00";
				ace_frame(217) <= x"00";
				ace_frame(218) <= x"00";
				ace_frame(219) <= x"00";
				ace_frame(220) <= x"00";
				ace_frame(221) <= x"00";
				ace_frame(222) <= x"00";
				ace_frame(223) <= x"00";
				ace_frame(224) <= x"00";
				ace_frame(225) <= x"00";
				ace_frame(226) <= x"00";
				ace_frame(227) <= x"00";
				ace_frame(228) <= x"00";
				ace_frame(229) <= x"00";
				ace_frame(230) <= x"00";
				ace_frame(231) <= x"00";
				ace_frame(232) <= x"00";
				ace_frame(233) <= x"00";
				ace_frame(234) <= x"00";
				
				-- trailing data
				-- 000c010000060000    // 8 bytes of Header and Synchronisation / State of Control-Channel
				-- 000c010000000000    // 8 bytes of Header and Synchronisation / State of Control-Channel
				-- 006832bb1867fce743  // 9 bytes of payload-data of control-channel
				-- eb000000            // 4 bytes with total message length (0xEB = 235 bytes)
				-- eb000000            // 4 bytes with total message length (0xEB = 235 bytes)
				ace_frame(235) <= x"00";
				ace_frame(236) <= x"0C";
				ace_frame(237) <= x"01";
				ace_frame(238) <= x"00";
				ace_frame(239) <= x"00";
				ace_frame(240) <= x"06";
				ace_frame(241) <= x"00";
				ace_frame(242) <= x"00";
				
				ace_frame(243) <= x"00";
				ace_frame(244) <= x"0C";
				ace_frame(245) <= x"01";
				ace_frame(246) <= x"00";
				ace_frame(247) <= x"00";
				ace_frame(248) <= x"00";
				ace_frame(249) <= x"00";
				ace_frame(250) <= x"00";

				ace_frame(251) <= x"00";
				ace_frame(252) <= x"68";
				ace_frame(253) <= x"32";
				ace_frame(254) <= x"BB";
				ace_frame(255) <= x"18";
				ace_frame(256) <= x"67";
				ace_frame(257) <= x"FC";
				ace_frame(258) <= x"E7";
				ace_frame(259) <= x"43";

				ace_frame(260) <= x"EB"; -- length of whole first part (MAC-Addresses + first payload-data)
				ace_frame(261) <= x"00";
				ace_frame(262) <= x"00";
				ace_frame(263) <= x"00";

				ace_frame(264) <= x"EB"; -- length of whole first part (MAC-Addresses + first payload-data)
				ace_frame(265) <= x"00";
				ace_frame(266) <= x"00";
				ace_frame(267) <= x"00";
				
				s_SM_Ethernet <= s_Start;
				
			elsif (s_SM_Ethernet = s_Start) then
				-- wait until MAC is ready again
				if (tx_busy = '0') then
					tx_enable <= '1';
					byte_counter <= 0; -- preload to first byte again
					tx_data <= ace_frame(0);

					s_SM_Ethernet <= s_Transmit;
				end if;
			
			elsif (s_SM_Ethernet = s_Transmit) then
				-- wait until previous byte is sent
				if (tx_byte_sent = '1') then
					-- send next byte and increment byte_counter
					tx_data <= ace_frame(byte_counter);
					
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
