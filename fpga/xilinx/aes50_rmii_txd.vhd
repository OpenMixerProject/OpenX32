library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity aes50_rmii_txd is
    Port ( 
        clk_in  		: in  STD_LOGIC;
        clk_in_inv  	: in  STD_LOGIC;
        reset  	 	: in  STD_LOGIC;
		  data		 	: in  STD_LOGIC_VECTOR(2 downto 0);
        rmii_tx_en	: out STD_LOGIC;
		  rmii_txd0		: out STD_LOGIC;
        rmii_txd1		: out STD_LOGIC
    );
end aes50_rmii_txd;

architecture Behavioral of aes50_rmii_txd is
begin
    ODDR2_inst0 : ODDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- NONE or SAME_EDGE
        INIT => '0',
        SRTYPE => "SYNC")
    port map (
        Q  => rmii_tx_en,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D0 => data(0),
        D1 => data(0),
        R  => reset,
        S  => '0'
    );

    ODDR2_inst1 : ODDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- NONE or SAME_EDGE
        INIT => '0',
        SRTYPE => "SYNC")
    port map (
        Q  => rmii_txd0,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D0 => data(1),
        D1 => data(1),
        R  => reset,
        S  => '0'
    );

    ODDR2_inst2 : ODDR2
    generic map(
        DDR_ALIGNMENT => "NONE", -- NONE or SAME_EDGE
        INIT => '0',
        SRTYPE => "SYNC")
    port map (
        Q  => rmii_txd1,
        C0 => clk_in,
        C1 => clk_in_inv,
        CE => '1',
        D0 => data(2),
        D1 => data(2),
        R  => reset,
        S  => '0'
    );
end Behavioral;