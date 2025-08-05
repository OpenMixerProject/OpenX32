library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_8ch_tx is
	port (
		clk 		: in std_logic; -- mainclock
		bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync		: in std_logic; -- Frame sync (for X32 it is 384 kHz)
		ch1_in	: in std_logic_vector(23 downto 0);
		ch2_in	: in std_logic_vector(23 downto 0);
		ch3_in	: in std_logic_vector(23 downto 0);
		ch4_in	: in std_logic_vector(23 downto 0);
		ch5_in	: in std_logic_vector(23 downto 0);
		ch6_in	: in std_logic_vector(23 downto 0);
		ch7_in	: in std_logic_vector(23 downto 0);
		ch8_in	: in std_logic_vector(23 downto 0);

		sdata		: out std_logic -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
		);
end tdm_8ch_tx;

architecture rtl of tdm_8ch_tx is
	signal sample_data	: std_logic_vector(32 * 8 - 1 downto 0) := (others=>'0');
	signal sdata_tmp		: std_logic;
	signal zbclk, zzbclk, zzzbclk		: std_logic;
	signal zfsync							: std_logic;
	signal bit_cnt							: integer range 0 to (32 * 8) + 1 := 0; -- one extra bit to prevent additional bitshift on following ch1 after ch8
begin
	process(clk)
	begin
		if rising_edge(clk) then
			zbclk <= bclk;
			zzbclk <= zbclk;
			zzzbclk <= zzbclk;

			if (zzbclk = '1' and zzzbclk = '0') then
				-- rising edge of bitclk

				-- check for positive edge of frame-sync (1 bit-clock before bit 0 of channel 1)
				if (fsync = '1' and zfsync = '0') then
					-- rising edge of sync-signal

					-- copy channel-data to output-register
					sample_data(sample_data'high - (32 * 0) downto sample_data'high - 23 - (32 * 0)) <= ch1_in;
					sample_data(sample_data'high - (32 * 1) downto sample_data'high - 23 - (32 * 1)) <= ch2_in;
					sample_data(sample_data'high - (32 * 2) downto sample_data'high - 23 - (32 * 2)) <= ch3_in;
					sample_data(sample_data'high - (32 * 3) downto sample_data'high - 23 - (32 * 3)) <= ch4_in;
					sample_data(sample_data'high - (32 * 4) downto sample_data'high - 23 - (32 * 4)) <= ch5_in;
					sample_data(sample_data'high - (32 * 5) downto sample_data'high - 23 - (32 * 5)) <= ch6_in;
					sample_data(sample_data'high - (32 * 6) downto sample_data'high - 23 - (32 * 6)) <= ch7_in;
					sample_data(sample_data'high - (32 * 7) downto sample_data'high - 23 - (32 * 7)) <= ch8_in;

					sdata_tmp <= ch1_in(ch1_in'high); -- set MSB of channel 1
					bit_cnt <= 1;
				else
					sdata_tmp <= sample_data(sample_data'high - bit_cnt);
					bit_cnt <= bit_cnt + 1;
				end if;
				zfsync <= fsync;
			end if;
		end if;

		if falling_edge(clk) then
			-- continuously outputing data from output-register on falling edge
			sdata <= sdata_tmp;
		end if;
	
	end process;
end rtl;
        




-- Sender for 8-channel TDM
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

--library ieee;
--use ieee.std_logic_1164.all;
--use ieee.std_logic_unsigned.all; 
--use ieee.numeric_std.all; -- lib for unsigned and signed
--
--entity tdm_8ch_tx is
--	port (
--		clk 		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
--		fsync		: in std_logic; -- Frame sync (for X32 it is 384 kHz)
--		ch1_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch2_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch3_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch4_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch5_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch6_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch7_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--		ch8_in	: in std_logic_vector(23 downto 0); -- received audio-sample
--
--		sdata		: out std_logic -- serial data (8x 32 bit audio-data: 24 bit of audio followed by 8 zero-bits)
--	  );
--end tdm_8ch_tx;
--
--architecture rtl of tdm_8ch_tx is
--	signal sample_data	: std_logic_vector(32 * 8 - 1 downto 0) := (others=>'0');
--	signal zfsync			: std_logic;
--	signal sdata_tmp		: std_logic;
--	signal bit_cnt			: integer range 0 to (32 * 8 - 1) := 0;
--begin
--	process(clk)
--	begin
--		if rising_edge(clk) then
--			-- check for positive edge of frame-sync
--			if fsync = '1' and zfsync = '0' then
--				-- we are at the last bit of the last channel
--				-- on next falling clock we will start sending with the the MSB of first channel
--
--				-- copy channel-data to output-register
--				sample_data(sample_data'high - (32 * 0) downto sample_data'high - 23 - (32 * 0)) <= ch1_in;
--				sample_data(sample_data'high - (32 * 1) downto sample_data'high - 23 - (32 * 1)) <= ch2_in;
--				sample_data(sample_data'high - (32 * 2) downto sample_data'high - 23 - (32 * 2)) <= ch3_in;
--				sample_data(sample_data'high - (32 * 3) downto sample_data'high - 23 - (32 * 3)) <= ch4_in;
--				sample_data(sample_data'high - (32 * 4) downto sample_data'high - 23 - (32 * 4)) <= ch5_in;
--				sample_data(sample_data'high - (32 * 5) downto sample_data'high - 23 - (32 * 5)) <= ch6_in;
--				sample_data(sample_data'high - (32 * 6) downto sample_data'high - 23 - (32 * 6)) <= ch7_in;
--				sample_data(sample_data'high - (32 * 7) downto sample_data'high - 23 - (32 * 7)) <= ch8_in;
--				
--				sdata_tmp <= ch1_in(ch1_in'high); -- set MSB of channel 1
--				bit_cnt <= 1; -- preload bit-counter to MSB-1 as we already output MSB with next falling edge
--			else
--				sdata_tmp <= sample_data(sample_data'high - bit_cnt);
--				bit_cnt <= bit_cnt + 1; -- increment bit-counter for the next cycle as we are using a signal instead of variable
--			end if;
--			zfsync <= fsync;
--		end if;
--
--		if falling_edge(clk) then
--			-- continuously outputing data from output-register on falling edge
--			sdata <= sdata_tmp;
--		end if;
--	end process;
--end rtl;
        