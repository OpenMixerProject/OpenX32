library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all; 
use ieee.numeric_std.all; -- lib for unsigned and signed

entity tdm_8ch_tx is
	port (
		clk 		: in std_logic; -- mainclock
		bclk		: in std_logic; -- bit-clock of TDM signal (for X32 it is 12.288 MHz)
		fsync		: in std_logic; -- Frame sync
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
	signal zfsync			: std_logic;
	signal bit_cnt			: integer range 0 to (32 * 8) - 1 := 0;
	signal sample_data	: std_logic_vector((32 * 8) - 1 downto 0) := (others => '0');
begin
	process(bclk)
	begin
		if rising_edge(bclk) then
			-- check for fsync
			if (fsync = '1' and zfsync = '0') then
				-- copy channel-data to output-register
				sample_data <= ch1_in & "00000000" & ch2_in & "00000000" & ch3_in & "00000000" & ch4_in & "00000000" & ch5_in & "00000000" & ch6_in & "00000000" & ch7_in & "00000000" & ch8_in & "00000000";

				-- output MSB of sample_data on next falling edge
				bit_cnt <= 0;
			else
				if (bit_cnt < ((32 * 8) - 1)) then
					bit_cnt <= bit_cnt + 1;
				end if;
			end if;

			zfsync <= fsync;
		end if;
	end process;

	process(bclk)
	begin
		if falling_edge(bclk) then
			sdata <= sample_data(sample_data'high - bit_cnt);
		end if;
	end process;
end rtl;
