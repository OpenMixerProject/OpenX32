library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity aes50_rmii_rxd is
    Port ( 
        clk_in  		: in  STD_LOGIC;
        clk_in_inv  	: in  STD_LOGIC;
        reset 			: in  STD_LOGIC;
        rmii_crs_dv	: in  STD_LOGIC;
        rmii_rxd0		: in  STD_LOGIC;
        rmii_rxd1		: in  STD_LOGIC;
        q      		: out STD_LOGIC_VECTOR(2 downto 0)
    );
end aes50_rmii_rxd;

architecture Behavioral of aes50_rmii_rxd is
begin
    -- IDDR2 Instanziierung speziell für Spartan-3A
    IDDR2_inst0 : IDDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- SAME_EDGE_PIPELINED if sampling of both inputs on rising edge is important
        INIT_Q0 => '0',
        INIT_Q1 => '0',
        SRTYPE => "SYNC")
    port map (
        Q0 => q(0),
        Q1 => open,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D  => rmii_crs_dv,
        R  => reset,
        S  => '0'
    );

    IDDR2_inst1 : IDDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- SAME_EDGE_PIPELINED if sampling of both inputs on rising edge is important
        INIT_Q0 => '0',
        INIT_Q1 => '0',
        SRTYPE => "SYNC")
    port map (
        Q0 => q(1),
        Q1 => open,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D  => rmii_rxd0,
        R  => reset,
        S  => '0'
    );

    IDDR2_inst2 : IDDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- SAME_EDGE_PIPELINED if sampling of both inputs on rising edge is important
        INIT_Q0 => '0',
        INIT_Q1 => '0',
        SRTYPE => "SYNC")
    port map (
        Q0 => q(2),
        Q1 => open,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D  => rmii_rxd1,
        R  => reset,
        S  => '0'
    );
end Behavioral;