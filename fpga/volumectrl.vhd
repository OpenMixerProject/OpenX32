library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity volumectrl is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_DSP_CHANNELS	: integer := 40
	);
	port (
		clk				: in std_logic := '0';
		sync_in			: in std_logic;
		audio_input		: in std_logic_vector(NUM_DSP_CHANNELS * DATA_WIDTH - 1 downto 0);
		volume_left		: in std_logic_vector(NUM_DSP_CHANNELS * 8 - 1 downto 0) := (others => '0'); -- audio-volume uses 8-bits
		volume_right	: in std_logic_vector(NUM_DSP_CHANNELS * 8 - 1 downto 0) := (others => '0'); -- audio-volume uses 8-bits

		audio_output	: out std_logic_vector(NUM_DSP_CHANNELS * DATA_WIDTH - 1 downto 0) := (others=>'0')
	);
end volumectrl;

architecture Behavioral of volumectrl is
	signal state		: natural range 0 to 30 := 0;
	signal choffset	: integer range 0 to NUM_DSP_CHANNELS * DATA_WIDTH := 0;
	signal voloffset	: integer range 0 to NUM_DSP_CHANNELS * 8 := 0;

	signal mult_in_sample	: signed(DATA_WIDTH - 1 downto 0) := (others=>'0');
	signal mult_in_vol		: signed(8 downto 0) := (others=>'0');
	signal mult_out			: signed(DATA_WIDTH + 8 downto 0) := (others=>'0');
begin
	process(mult_in_sample, mult_in_vol)
	begin
		mult_out <= mult_in_sample * mult_in_vol;
	end process;

	process(clk)
	begin
		if rising_edge(clk) then
			--if (sync_in = '1') then
			--	audio_output <= audio_input;
			--end if;
		
			if (sync_in = '1' and state = 0) then
				-- load first audio-channel and volume into multiplicator
				mult_in_sample <= signed(audio_input(DATA_WIDTH - 1 downto 0));
				mult_in_vol <= signed("0" & volume_left(7 downto 0));
				
				-- preload pointers for next rising edge
				choffset <= DATA_WIDTH;
				voloffset <= 8;
				
				state <= 1;
			elsif (state = 1) then
				-- copy processed audio-sample to output
				audio_output(choffset - 1 downto choffset - 24) <= std_logic_vector(resize(shift_right(mult_out, 7), DATA_WIDTH));

				-- check if we have more to process
				if (choffset < (NUM_DSP_CHANNELS - 1) * DATA_WIDTH) then
					-- load multiplicator with next sample
					mult_in_sample <= signed(audio_input(choffset + DATA_WIDTH - 1 downto choffset));
					mult_in_vol <= signed("0" & volume_left(voloffset + 8 - 1 downto voloffset));
					
					choffset <= choffset + DATA_WIDTH;
					voloffset <= voloffset + 8;
				else
					-- exit
					state <= 0;
				end if;
			end if;
		end if;
	end process;
end Behavioral;