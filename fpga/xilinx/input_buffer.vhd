library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library unisim;
use unisim.vcomponents.all;

entity input_buffer is
	port(
		clock_i  : in  std_ulogic;
		pad_i    : in  std_ulogic;

		buffer_o : out std_ulogic
	);
end entity;

architecture rtl of input_buffer is
	signal delayed : std_ulogic := '0';

	-- Force putting input Flip-Flop into IOB so it doesn't end up in a normal logic tile
	-- which would ruin the timing.
	attribute iob : string;
	attribute iob of FDRE_inst : label is "FORCE";
begin
	FDRE_inst : FDRE
		generic map(
			INIT => '0')                -- Initial value of register ('0' or '1')  
		port map(
			Q  => buffer_o,             -- Data output
			C  => clock_i,              -- Clock input
			CE => '1',                  -- Clock enable input
			R  => '0',                  -- Synchronous reset input
			D  => pad_i                 -- Data input
		);

end architecture;
