-- Receiver for 8-channel TDM
-- (c) 2024 Dr.-Ing. Christian Noeding
-- christian@noeding-online.de
-- Released under GNU General Public License v3
-- Source: https://www.github.com/xn--nding-jua/xfbape
--
-- This file contains a TDM-receiver for the Behringer Expansion Cards
-- TDM (Time Division Multiplexed Audio Interface) sends 8 24-bit audio-samples
-- plus 8 additional zero-padding bits right after each other.
-- It uses two additional signals: LR-Clock (Frame Sync) and the serial-clock (bit-clock)
--
-- More information have look at https://gab.wallawalla.edu/~larry.aamodt/engr432/cirrus_logic_TDM_AN301.pdf

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_8ch_rx is
	port (
		clk 		: in std_logic; -- mainclock
		bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync		: in std_logic; -- Frame sync (for X32 it is 384 kHz)
		sdata		: in std_logic; -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
		
		ch1_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch2_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch3_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch4_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch5_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch6_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch7_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		ch8_out		: out std_logic_vector(23 downto 0); -- received audio-sample
		sync_out		: out std_logic -- new data received successfully
	  );
end tdm_8ch_rx;

architecture rtl of tdm_8ch_rx is
	signal zfsync				: std_logic;
	signal bit_cnt				: integer range 0 to (32 * 8) - 1 := 0;
	signal sample_data		: std_logic_vector((32 * 8) - 1 downto 0) := (others => '0');
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- continuously reading bit into shift-register
			sample_data <= sample_data(sample_data'left - 1 downto 0) & sdata;
			
			-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
			if (fsync = '1' and zfsync = '0') then
				-- reading LSB of channel 8

				-- set bit_cnt for next MSB
				bit_cnt <= 0;
			else
				bit_cnt <= bit_cnt + 1;
			end if;
			
			if (bit_cnt = 0) then
				-- we are reading MSB of ch1, so all channel-bits are within sample_data
				ch1_out <= sample_data(31 + (32*7) downto 8 + (32*7));
				ch2_out <= sample_data(31 + (32*6) downto 8 + (32*6));
				ch3_out <= sample_data(31 + (32*5) downto 8 + (32*5));
				ch4_out <= sample_data(31 + (32*4) downto 8 + (32*4));
				ch5_out <= sample_data(31 + (32*3) downto 8 + (32*3));
				ch6_out <= sample_data(31 + (32*2) downto 8 + (32*2));
				ch7_out <= sample_data(31 + (32*1) downto 8 + (32*1));
				ch8_out <= sample_data(31 + (32*0) downto 8 + (32*0));

				sync_out <= '1'; -- tell everyone that we have new set of audio-samples
			else
				sync_out <= '0';
			end if;

			zfsync <= fsync;
		end if;
	end process;
end rtl;
        