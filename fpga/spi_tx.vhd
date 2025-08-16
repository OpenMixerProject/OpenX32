-- VHDL File to write to a SPI-device
-- v0.0.1, 24.07.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_tx is
	port (
		clk			: in std_logic; -- expecting 16 MHz
		i_address	: in std_logic_vector(7 downto 0);
		i_map			: in std_logic_vector(7 downto 0);
		i_data		: in std_logic_vector(7 downto 0);
		i_start		: in std_logic;
		
		o_nCS			: out std_logic;
		o_cclk		: out std_logic;
		o_cdata		: out std_logic;
		o_busy		: out std_logic
	);
end entity;

architecture behavioral of spi_tx is
	type t_SM is (s_Idle, s_Tx, s_Cleanup);
	signal s_SM			: t_SM := s_Idle;
	
	signal startup		: std_logic := '1';
	signal count_clk	: natural range 0 to 1 := 0;
	signal nCS			: std_logic := '0';
	signal cclk			: std_logic := '0';
	signal cdata		: std_logic := '0';
	signal tx_data		: std_logic_vector(23 downto 0);
	signal tx_counter	: natural range 0 to 24 := 0;
begin
	process (clk)
	begin
		if rising_edge(clk) then
			if (s_SM = s_Idle) then
				if (i_start = '1') then
					-- activate chip
					nCS <= '0';
					o_busy <= '1';
					startup <= '1';
					
					-- copy data to output-buffer
					tx_data(23 downto 16) <= i_address;
					tx_data(15 downto 8) <= i_map;
					tx_data(7 downto 0) <= i_data;
					tx_counter <= 0;

					-- go into transmission-state
					s_SM <= s_Tx;
				else
					nCS <= '1';
					cclk <= '0';
					cdata <= '0';
				end if;
				
			elsif (s_SM = s_Tx) then
				-- transmit at around 4 MHz (CS42438 is able to handle SPI at max. 6 MHz)
				if (count_clk = 1 or startup = '1') then -- divide clk by 4 (16 MHz / 4 = 4 MHz)
					if (cclk = '0' and startup = '0') then
						-- we have rising edge
						cclk <= '1';

						-- nothing to do here
					else
						-- we have falling edge
						cclk <= '0';
						startup <= '0';
						
						-- check if we have reached end of message
						if (tx_counter = 24) then
							tx_counter <= 0;
							s_SM <= s_Cleanup;
						else
							-- output bits until we reached end of message
							cdata <= tx_data(23 - tx_counter); -- send MSB first
							tx_counter <= tx_counter + 1;
						end if;
					end if;
					count_clk <= 0;
				else
					count_clk <= count_clk + 1;
				end if;
				
			elsif (s_SM = s_Cleanup) then
				nCS <= '1';
				cclk <= '0';
				cdata <= '0';
				o_busy <= '0';
				
				-- go back to idle-state
				s_SM <= s_Idle;
				
			end if;		
		end if;
	end process;
	
	o_nCS <= nCS;
	o_cclk <= cclk;
	o_cdata <= cdata;
end behavioral;