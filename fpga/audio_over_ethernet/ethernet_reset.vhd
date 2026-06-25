library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;

entity ethernet_reset is 
	port
	(
		clk				: in std_logic;
		power_good		: in std_logic;

		phy_rstn			: out std_logic; -- reset for PHY
		mac_rst			: out std_logic; -- reset for MAC
		sendArpRequest	: out std_logic;
		tx_online		: out std_logic
	);
end entity;

architecture Behavioral of ethernet_reset is
	type t_SM_Ethernet is (s_Wait, s_Reset, s_Wait2, s_Wait3, s_Wait4, s_ArpRequest, s_Wait5, s_Done);
	signal s_SM_Ethernet 	: t_SM_Ethernet := s_Wait;
	signal counter : integer range 0 to 30000 := 0; -- allow up to 30 seconds
begin
	process (clk)
	begin
		if (rising_edge(clk)) then
			if s_SM_Ethernet = s_Wait then
				sendArpRequest <= '0';
				phy_rstn <= '1';
				mac_rst <= '0';
				tx_online <= '0';
				
				if (power_good = '1') then
					counter <= counter + 1;
				else
					counter <= 0;
				end if;
				
				-- wait 4 ms
				if (counter > 4) then
					s_SM_Ethernet <= s_Reset;
				end if;
			elsif s_SM_Ethernet = s_Reset then
				-- rise reset signals
				phy_rstn <= '0';
				mac_rst <= '1';
				counter <= 0;
				
				s_SM_Ethernet <= s_Wait2;
				
			elsif s_SM_Ethernet = s_Wait2 then
				-- wait 4 ms
				if (counter < 4) then
					counter <= counter + 1;
				else
					counter <= 0;
					s_SM_Ethernet <= s_Wait3;
				end if;
				
			elsif s_SM_Ethernet = s_Wait3 then
				-- disable reset signal
				phy_rstn <= '1';
				mac_rst <= '0';

				counter <= counter + 1;
				-- wait 4 ms
				if (counter > 4) then
					counter <= 0;
					s_SM_Ethernet <= s_Wait4;
				end if;
				
			elsif s_SM_Ethernet = s_Wait4 then

				counter <= counter + 1;
				-- wait 3 seconds
				if (counter > 3000) then
					counter <= 0;
					s_SM_Ethernet <= s_ArpRequest;
				end if;

			elsif s_SM_Ethernet = s_ArpRequest then
				sendArpRequest <= '1';
				counter <= 0;
				s_SM_Ethernet <= s_Wait5;

			elsif s_SM_Ethernet = s_Wait5 then
				sendArpRequest <= '0';
				
				counter <= counter + 1;
				-- wait 3 seconds
				if (counter > 3000) then
					counter <= 0;
					s_SM_Ethernet <= s_Done;
				end if;

			elsif s_SM_Ethernet = s_Done then
				tx_online <= '1';
				-- stay here forever
			end if;
		end if;
	end process;
end Behavioral;