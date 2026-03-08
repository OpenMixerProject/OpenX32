library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_spi_continuous is
    port (
        clk         : in  std_logic;
        reset       : in  std_logic;
        -- Eingang vom Decoder
        bit_in      : in  std_logic;
        valid_in    : in  std_logic;
        
        -- SPI Interface (Kontinuierlich)
        spi_sclk    : in  std_logic; -- Der externe Master taktet durchgehend
        spi_miso    : out std_logic
    );
end entity;

architecture rtl of aes50_spi_continuous is
    -- FIFO Signale
    type fifo_array is array (0 to 31) of std_logic_vector(7 downto 0);
    signal fifo_mem     : fifo_array := (others => (others => '0'));
    signal wr_ptr       : unsigned(4 downto 0) := (others => '0');
    signal rd_ptr       : unsigned(4 downto 0) := (others => '0');
    
    -- Byte Assembler
    signal fill_byte    : std_logic_vector(7 downto 0);
    signal fill_cnt     : integer range 0 to 7 := 0;

    -- SPI Schieberegister
    signal shift_reg    : std_logic_vector(7 downto 0) := (others => '0');
    signal bit_idx      : unsigned(2 downto 0) := "111"; -- Zählt von 7 runter zu 0
    signal sclk_sync    : std_logic_vector(2 downto 0); -- Zur Flankenerkennung
begin

    -- 1. FIFO-Schreibseite: Bits zu Bytes sammeln
    process(clk, reset)
    begin
        if reset = '1' then
            fill_cnt <= 0;
            wr_ptr <= (others => '0');
        elsif rising_edge(clk) then
            if valid_in = '1' then
                fill_byte <= fill_byte(6 downto 0) & bit_in;
                if fill_cnt = 7 then
                    fifo_mem(to_integer(wr_ptr)) <= fill_byte(6 downto 0) & bit_in;
                    wr_ptr <= wr_ptr + 1;
                    fill_cnt <= 0;
                else
                    fill_cnt <= fill_cnt + 1;
                end if;
            end if;
        end if;
    end process;

    -- 2. SPI-Ausgabeseite (Synchronisiert auf clk zur Vermeidung von Metastabilität)
    process(clk, reset)
    begin
        if reset = '1' then
            sclk_sync <= (others => '0');
            bit_idx <= "111";
            rd_ptr <= (others => '0');
        elsif rising_edge(clk) then
            sclk_sync <= sclk_sync(1 downto 0) & spi_sclk;
            
            -- Fallende Flanke von SCLK: Bit ausgeben
            if sclk_sync(2 downto 1) = "10" then
                if bit_idx = 0 then
                    -- Byte fertig übertragen, lade nächstes Byte
                    bit_idx <= "111";
                    if rd_ptr /= wr_ptr then
                        shift_reg <= fifo_mem(to_integer(rd_ptr));
                        rd_ptr <= rd_ptr + 1;
                    else
                        shift_reg <= x"00"; -- Idle-Byte wenn FIFO leer
                    end if;
                else
                    -- Normales Shiften (MSB first)
                    shift_reg <= shift_reg(6 downto 0) & '0';
                    bit_idx <= bit_idx - 1;
                end if;
            end if;
        end if;
    end process;

    spi_miso <= shift_reg(7);

end architecture;
