-- Block-RAM-Module for Audio-Routing-Matrix
-- v0.1.0, 08.08.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix_ram is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
	);
	port (
		clk				: in std_logic;
		read_addr		: in unsigned(6 downto 0); -- log2(112) = 7
		write_addr		: in unsigned(6 downto 0); -- log2(112) = 7
		i_data			: in std_logic_vector(DATA_WIDTH - 1 downto 0);
		wr_en				: in std_logic;

		o_data			: out  std_logic_vector(DATA_WIDTH - 1 downto 0)
	);
end entity audiomatrix_ram;

architecture behavioral of audiomatrix_ram is
	type ram_type is array (127 downto 0) of std_logic_vector(DATA_WIDTH - 1 downto 0); -- we are using only 112 elements, but we take 2^7 RAM-elements to match the full address-range of 7-bit address-pointers
	signal ram_inst : ram_type;
begin
	process(clk)
	begin
		if falling_edge(clk) then
			-- write data to RAM
			if wr_en = '1' then
				ram_inst(to_integer(write_addr)) <= i_data;
			end if;

			-- read data from RAM
			o_data <= ram_inst(to_integer(read_addr));
		end if;
	end process;
end architecture behavioral;