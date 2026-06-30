library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity oddr_clock is
    Port ( 
        clk_in  : in  STD_LOGIC;
        clk_in_inv  : in  STD_LOGIC;
        reset   : in  STD_LOGIC;
		  d0		 : in  STD_LOGIC;
		  d1		 : in  STD_LOGIC;
        Q : out STD_LOGIC
    );
end oddr_clock;

architecture Behavioral of oddr_clock is
begin
    -- ODDR2 Instanziierung speziell für Spartan-3A
    ODDR2_inst : ODDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- NONE or SAME_EDGE
        INIT => '0',
        SRTYPE => "SYNC")
    port map (
        Q  => Q,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D0 => d0,
        D1 => d1,
        R  => reset,
        S  => '0'
    );
end Behavioral;