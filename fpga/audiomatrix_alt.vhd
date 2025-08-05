-- VHDL File to route multiple audio-inputs to multiple audio-outputs
-- v0.0.2, 05.08.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 
-- The current solution takes a lot of LUTs. Better option would be to use Block-RAM
-- TODO:
-- 1. write incoming audio-data to block-ram
-- 2. generate read-address for all 112 signals based on select_lines
-- 3. read outgoing audio-data from block-ram to individual slices

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		NUM_OUTPUT_PORTS	: integer := 112	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input
	);
	port (
		clk					: in std_logic;
		sync_in				: in std_logic;
		input_data			: in  std_logic_vector(NUM_INPUT_PORTS * DATA_WIDTH - 1 downto 0);
		select_lines		: in  std_logic_vector(NUM_OUTPUT_PORTS * 8 - 1 downto 0); -- log2(32) = 5, log2(72) = 7, log2(112) = 7
		output_data			: out std_logic_vector(NUM_OUTPUT_PORTS * DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix;

architecture behavioral of audiomatrix is
	signal counter			: integer range 0 to NUM_OUTPUT_PORTS;
	signal pSelector		: integer range 0 to NUM_OUTPUT_PORTS * 8;
	signal pOutput			: integer range 0 to NUM_OUTPUT_PORTS * DATA_WIDTH;
begin
	process(clk)
		variable pInput	: integer range 0 to NUM_INPUT_PORTS * DATA_WIDTH;
	begin
		if rising_edge(clk) then
			if (sync_in = '1') then
				-- new audio-samples have reached
				counter <= 0;
				pSelector <= 0;
				pOutput <= 0;
			else
				if (counter < NUM_INPUT_PORTS) then
					-- calculate the read-pointer based on select-line
					pInput := to_integer(unsigned(select_lines(pSelector + 8 - 1 downto pSelector))) * 24; -- warning about cropped multiplication to 12 bits can be ignored for now
				
					-- copy data from input-vector to output-vector
					output_data(pOutput + 24 - 1 downto pOutput) <= input_data(pInput + 24 - 1 downto pInput); -- pInput can be up to 255 * 24 = 6120. Synthesizer will give us a warning here, but it can be ignored for now
					
					-- increment the pointers for next clock-cycle
					counter <= counter + 1;
					pSelector <= pSelector + 8;
					pOutput <= pOutput + 24;
				end if;
			end if;
		end if;		
	end process;
end architecture behavioral;
