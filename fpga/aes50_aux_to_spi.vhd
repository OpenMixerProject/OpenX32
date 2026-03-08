library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_aux_to_spi is
    port (
        clk         : in  std_logic;
        reset       : in  std_logic;
        -- Eingang vom Decoder-Modul
        bit_in      : in  std_logic;
        valid_in    : in  std_logic;
        
        -- SPI Interface (Slave)
        spi_sclk    : in  std_logic;
        spi_ss      : in  std_logic;
        spi_miso    : out std_logic
    );
end entity;

architecture rtl of aes50_aux_to_spi is
    -- Puffer für Byte-Assemblierung
    signal byte_shifter : std_logic_vector(7 downto 0);
    signal bit_cnt      : integer range 0 to 7 := 0;
    
    -- Einfacher FIFO (Ringpuffer)
    type fifo_array is array (0 to 15) of std_logic_vector(7 downto 0);
    signal fifo_mem     : fifo_array;
    signal wr_ptr       : unsigned(3 downto 0) := (others => '0');
    signal rd_ptr       : unsigned(3 downto 0) := (others => '0');
    
    -- SPI Signale
    signal spi_tx_reg   : std_logic_vector(7 downto 0);
    signal sclk_reg     : std_logic_vector(1 downto 0);

begin

    -- 1. Bits zu Bytes sammeln
    process(clk, reset)
    begin
        if reset = '1' then
            bit_cnt <= 0;
            wr_ptr <= (others => '0');
        elsif rising_edge(clk) then
            if valid_in = '1' then
                -- AES50 ist meist MSB first im Aux-Stream
                byte_shifter <= byte_shifter(6 downto 0) & bit_in;
                
                if bit_cnt = 7 then
                    bit_cnt <= 0;
                    fifo_mem(to_integer(wr_ptr)) <= byte_shifter(6 downto 0) & bit_in;
                    wr_ptr <= wr_ptr + 1;
                else
                    bit_cnt <= bit_cnt + 1;
                end if;
            end if;
        end if;
    end process;

    -- 2. SPI Slave Logik (einfach)
    -- Hinweis: Dies ist eine vereinfachte Darstellung. 
    -- Für echtes High-Speed SPI sollte sclk synchronisiert werden.
    process(spi_sclk, spi_ss)
    begin
        if spi_ss = '1' then
            -- Wenn SS inaktiv, lade das nächste Byte aus dem FIFO
            if rd_ptr /= wr_ptr then
                spi_tx_reg <= fifo_mem(to_integer(rd_ptr));
            else
                spi_tx_reg <= x"00"; -- Idle/Empty Pattern
            end if;
        elsif falling_edge(spi_sclk) then
            -- Shiften bei fallender Flanke (Mode 0)
            spi_tx_reg <= spi_tx_reg(6 downto 0) & '0';
        end if;
    end process;

    -- Das MSB liegt immer an MISO an
    spi_miso <= spi_tx_reg(7) when spi_ss = '0' else 'Z';

    -- FIFO-Lesezeiger inkrementieren, wenn ein Byte fertig übertragen wurde
    process(spi_ss)
    begin
        if rising_edge(spi_ss) then
            if rd_ptr /= wr_ptr then
                rd_ptr <= rd_ptr + 1;
            end if;
        end if;
    end process;

end architecture;
