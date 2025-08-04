-- VHDL File to route multiple audio-inputs to multiple audio-outputs
-- v0.0.1, 04.08.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32
-- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audio_matrix is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		NUM_OUTPUT_PORTS	: integer := 112	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input
	);
	port (
		input_data			: in  std_logic_vector(NUM_INPUT_PORTS * DATA_WIDTH - 1 downto 0);
		select_lines		: in  std_logic_vector(NUM_OUTPUT_PORTS * 7 - 1 downto 0); -- log2(32) = 5, log2(72) = 7, log2(112) = 7
		output_data			: out std_logic_vector(NUM_OUTPUT_PORTS * DATA_WIDTH - 1 downto 0)
	);
end entity audio_matrix;

architecture behavioral of audio_matrix is
	function log2(n: positive) return integer is
		variable res		: integer := 0;
		variable m			: positive := n - 1;
	begin
		while m > 0 loop
			m := m / 2;
			res := res + 1;
		end loop;
		return res;
	end function log2;

	-- Signale für die Arrays der Vektoren zur Vereinfachung der Logik
	type input_data_array_t is array (0 to NUM_INPUT_PORTS - 1) of std_logic_vector(DATA_WIDTH - 1 downto 0);
	type output_data_array_t is array (0 to NUM_OUTPUT_PORTS - 1) of std_logic_vector(DATA_WIDTH - 1 downto 0);
	type select_lines_array_t is array (0 to NUM_OUTPUT_PORTS - 1) of std_logic_vector(log2(NUM_INPUT_PORTS) - 1 downto 0);

	signal input_array	: input_data_array_t;
	signal output_array	: output_data_array_t;
	signal select_array	: select_lines_array_t;
begin
	-- convert flat input-vector into array of vectors
	gen_input_arrays: for i in 0 to NUM_INPUT_PORTS - 1 generate
		input_array(i) <= input_data((i+1)*DATA_WIDTH - 1 downto i*DATA_WIDTH);
	end generate gen_input_arrays;

	gen_select_arrays: for i in 0 to NUM_OUTPUT_PORTS - 1 generate
		select_array(i) <= select_lines((i+1)*log2(NUM_INPUT_PORTS) - 1 downto i*log2(NUM_INPUT_PORTS));
	end generate gen_select_arrays;

	gen_output_arrays: for i in 0 to NUM_OUTPUT_PORTS - 1 generate
		output_data((i+1)*DATA_WIDTH - 1 downto i*DATA_WIDTH) <= output_array(i);
	end generate gen_output_arrays;

	-- set input-vectors to desired output-vectors
	gen_muxes: for i in 0 to NUM_OUTPUT_PORTS - 1 generate
		output_array(i) <= input_array(to_integer(unsigned(select_array(i))));
	end generate gen_muxes;
end architecture behavioral;