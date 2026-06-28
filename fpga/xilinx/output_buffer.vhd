library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library unisim;
use unisim.vcomponents.all;

entity output_buffer is
	port(
		clock_i  : in  std_ulogic;
		buffer_i : in  std_ulogic;

		pad_o    : out std_ulogic
	);
end entity;

architecture rtl of output_buffer is
	-- Force putting input Flip-Flop into IOB so it doesn't end up in a normal logic tile
	-- which would ruin the timing.
	attribute iob : string;
	attribute iob of FDRE_inst : label is "FORCE";
begin
	FDRE_inst : FDRE
		generic map(
			INIT => '0')                -- Initial value of register ('0' or '1')  
		port map(
			Q  => pad_o,                -- Data output
			C  => clock_i,              -- Clock input
			CE => '1',                  -- Clock enable input
			R  => '0',                  -- Synchronous reset input
			D  => buffer_i              -- Data input
		);
end architecture;
