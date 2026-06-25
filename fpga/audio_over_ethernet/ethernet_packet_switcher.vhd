library ieee;
use ieee.std_logic_1164.all;

entity ethernet_packet_switcher is
	port
	(
		select_i		: in std_logic; -- select between 2 packets

		tx_en0_i		: in std_logic;
		data0_i		: in std_logic_vector(7 downto 0);
		tx_en1_i		: in std_logic;
		data1_i		: in std_logic_vector(7 downto 0);

		tx_en_o		: out std_logic;
		data_o		: out std_logic_vector(7 downto 0)
	);
end entity;

architecture Behavioral of ethernet_packet_switcher is
begin
	process(select_i, tx_en0_i, data0_i, tx_en1_i, data1_i)
	begin
		if (select_i = '0') then
			tx_en_o <= tx_en0_i;
			data_o <= data0_i;
		else
			tx_en_o <= tx_en1_i;
			data_o <= data1_i;
		end if;
	end process;
end Behavioral;