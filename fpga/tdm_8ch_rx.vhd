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
	signal zbclk		: std_logic;
	signal zfsync		: std_logic;
	signal bit_cnt		: integer range 0 to (32 * 8) + 1 := 0; -- one extra bit to prevent additional bitshift on following ch1 after ch8
	signal sync_out_tmp	: std_logic;
	
	signal ch1	: std_logic_vector(31 downto 0);
	signal ch2	: std_logic_vector(31 downto 0);
	signal ch3	: std_logic_vector(31 downto 0);
	signal ch4	: std_logic_vector(31 downto 0);
	signal ch5	: std_logic_vector(31 downto 0);
	signal ch6	: std_logic_vector(31 downto 0);
	signal ch7	: std_logic_vector(31 downto 0);
	signal ch8	: std_logic_vector(31 downto 0);
begin
	process(clk)
	begin
		if rising_edge(clk) then
			zbclk <= bclk;

			if (bclk = '1' and zbclk = '0') then
				-- rising edge of bitclk

				-- continuously reading bit into shift-register
				if ((bit_cnt >= 0) and (bit_cnt <= 31)) then
					ch1 <= ch1(ch1'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 32) and (bit_cnt <= 63)) then
					ch2 <= ch2(ch2'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 64) and (bit_cnt <= 95)) then
					ch3 <= ch3(ch3'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 96) and (bit_cnt <= 127)) then
					ch4 <= ch4(ch4'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 128) and (bit_cnt <= 159)) then
					ch5 <= ch5(ch5'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 160) and (bit_cnt <= 191)) then
					ch6 <= ch6(ch6'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 192) and (bit_cnt <= 223)) then
					ch7 <= ch7(ch7'high - 1 downto 0) & sdata;
				elsif ((bit_cnt >= 224) and (bit_cnt <= 255)) then
					ch8 <= ch8(ch8'high - 1 downto 0) & sdata;
				end if;

				if (bit_cnt = 0) then
					-- we are reading MSB of ch1, so all channel-bits are within vectors
					ch1_out <= ch1(31 downto 8);
					ch2_out <= ch2(31 downto 8);
					ch3_out <= ch3(31 downto 8);
					ch4_out <= ch4(31 downto 8);
					ch5_out <= ch5(31 downto 8);
					ch6_out <= ch6(31 downto 8);
					ch7_out <= ch7(31 downto 8);
					ch8_out <= ch8(31 downto 8);
					sync_out_tmp <= '1'; -- will be resseted one clk later
				end if;

				-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
				if (fsync = '1' and zfsync = '0') then
					-- rising edge of sync-signal (while reading LSB of ch8)
					bit_cnt <= 0; -- reset bit-counter
				else
					bit_cnt <= bit_cnt + 1;
				end if;
				
				zfsync <= fsync;
			end if;
			
			if (sync_out_tmp = '1') then
				sync_out_tmp <= '0';
			end if;
		end if;
	end process;

	sync_out <= sync_out_tmp;
end rtl;
        