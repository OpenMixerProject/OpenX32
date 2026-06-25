library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

use work.ethernet_types.all;
use work.utility.all;

entity reverse_mac is
	port
	(
		mac_address_i	: in std_logic_vector(47 downto 0);
		mac_address_o	: out std_ulogic_vector(47 downto 0)
	);
end entity;

architecture Behavioral of reverse_mac is
begin
	mac_address_o <= reverse_bytes(std_ulogic_vector(mac_address_i));
end Behavioral;