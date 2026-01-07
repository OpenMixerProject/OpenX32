library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity oddr_clock is
    Port ( 
        clk_in  : in  STD_LOGIC;
        reset   : in  STD_LOGIC;
        clk_out : out STD_LOGIC
    );
end oddr_clock;

architecture Behavioral of oddr_clock is
begin
    -- ODDR2 Instanziierung speziell für Spartan-3A
    ODDR2_inst : ODDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- Erlaubt C0 und C1 separat zu nutzen
        INIT => '0',             -- Initialer Ausgangszustand
        SRTYPE => "SYNC")        -- Synchroner Reset
    port map (
        Q  => clk_out,
        C0 => clk_in,            -- Der normale Takt
        C1 => not clk_in,        -- Der invertierte Takt (lokale Invertierung im IOB)
        CE => '1',
        D0 => '1',               -- Entspricht D1 beim neueren ODDR
        D1 => '0',               -- Entspricht D2 beim neueren ODDR
        R  => reset,
        S  => '0'
    );
end Behavioral;