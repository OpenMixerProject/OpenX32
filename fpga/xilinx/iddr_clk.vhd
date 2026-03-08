library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity iddr_clock is
    Port ( 
        clk_in  : in  STD_LOGIC;
        clk_in_inv  : in  STD_LOGIC;
        reset   : in  STD_LOGIC;
        d       : in  STD_LOGIC;
        q0      : out STD_LOGIC;
        q1      : out STD_LOGIC
    );
end iddr_clock;

architecture Behavioral of iddr_clock is
begin
    -- IDDR2 Instanziierung speziell für Spartan-3A
    IDDR2_inst : IDDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- Erlaubt C0 und C1 separat zu nutzen
        INIT_Q0 => '0',
        INIT_Q1 => '0',
        SRTYPE => "SYNC")
    port map (
        Q0 => q0,
        Q1 => q1,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D  => d,
        R  => reset,
        S  => '0'
    );
end Behavioral;