library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity const_eth_config is
	generic
	(
		SRC_MAC0		: integer := 0;   -- AA = 00 = demo MAC-Address from Intel Triple Speed Ethernet example
		SRC_MAC1		: integer := 28;  -- BB = 1c
		SRC_MAC2		: integer := 35;  -- CC = 23
		SRC_MAC3		: integer := 23;  -- DD = 17
		SRC_MAC4		: integer := 74;  -- EE = 4a
		SRC_MAC5		: integer := 203; -- FF = cb

		DST_MAC0		: integer := 0;   -- AA = 00
		DST_MAC1		: integer := 36;  -- BB = 24
		DST_MAC2		: integer := 155; -- CC = 9B
		DST_MAC3		: integer := 127; -- DD = 7F
		DST_MAC4		: integer := 185; -- EE = B9
		DST_MAC5		: integer := 42;  -- FF = 2A
		
		SRC_IP0		: integer := 192;
		SRC_IP1		: integer := 168;
		SRC_IP2		: integer := 50;
		SRC_IP3		: integer := 99;
		SRC_PORT		: integer := 4023;
		
		DST_IP0		: integer := 192;
		DST_IP1		: integer := 168;
		DST_IP2		: integer := 50;
		DST_IP3		: integer := 98;
		DST_PORT		: integer := 4023
	);
	port
	(
		src_mac_address	: out std_logic_vector(47 downto 0);
		src_ip_address		: out std_logic_vector(31 downto 0);
		dst_mac_address	: out std_logic_vector(47 downto 0);
		dst_ip_address		: out std_logic_vector(31 downto 0);
		src_udp_port		: out std_logic_vector(15 downto 0);
		dst_udp_port		: out std_logic_vector(15 downto 0)
	);
end entity;

architecture Behavioral of const_eth_config is
begin
	-- MAC-Addresses like AA-BB-CC-DD-EE-FF will be stored in this manner: 0xaabbccddeeff, so MSB contains AA, LSB contains FF
	-- mac_address <= x"001c23174acb"; -- demo MAC-Address from Intel Triple Speed Ethernet example
	src_mac_address <= std_logic_vector(to_unsigned(SRC_MAC0, 8)) & std_logic_vector(to_unsigned(SRC_MAC1, 8)) & std_logic_vector(to_unsigned(SRC_MAC2, 8)) & std_logic_vector(to_unsigned(SRC_MAC3, 8)) & std_logic_vector(to_unsigned(SRC_MAC4, 8)) & std_logic_vector(to_unsigned(SRC_MAC5, 8));
	dst_mac_address <= std_logic_vector(to_unsigned(DST_MAC0, 8)) & std_logic_vector(to_unsigned(DST_MAC1, 8)) & std_logic_vector(to_unsigned(DST_MAC2, 8)) & std_logic_vector(to_unsigned(DST_MAC3, 8)) & std_logic_vector(to_unsigned(DST_MAC4, 8)) & std_logic_vector(to_unsigned(DST_MAC5, 8));

	-- IP-Addresses like 192.168.0.1 will be stored in this manner: 0xc0a80001, so MSB contains 192, LSB contains 1
	src_ip_address <= std_logic_vector(to_unsigned(SRC_IP0, 8)) & std_logic_vector(to_unsigned(SRC_IP1, 8)) & std_logic_vector(to_unsigned(SRC_IP2, 8)) & std_logic_vector(to_unsigned(SRC_IP3, 8));
	dst_ip_address <= std_logic_vector(to_unsigned(DST_IP0, 8)) & std_logic_vector(to_unsigned(DST_IP1, 8)) & std_logic_vector(to_unsigned(DST_IP2, 8)) & std_logic_vector(to_unsigned(DST_IP3, 8));

	src_udp_port <= std_logic_vector(to_unsigned(SRC_PORT, 16));
	dst_udp_port <= std_logic_vector(to_unsigned(DST_PORT, 16));
end Behavioral;