library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_aux_decoder is
    port (
        clk         : in  std_logic;
        reset       : in  std_logic;
        tdm_bclk    : in  std_logic;
        tdm_wclk    : in  std_logic;
        tdm_aux_i   : in  std_logic; -- Hier dein "tdm_aux_o" Signal anschließen
        
        data_out    : out std_logic;
        valid_out   : out std_logic
    );
end entity;

architecture rtl of aes50_aux_decoder is
    signal bclk_reg : std_logic_vector(2 downto 0);
    signal wclk_reg : std_logic_vector(2 downto 0);
    
    signal bit_in_slot : integer range 0 to 31 := 0;
    
    -- De-Stuffing & Descrambler
    signal ones_count : integer range 0 to 10 := 0;
    signal desc_reg   : std_logic_vector(8 downto 0) := (others => '0');
begin

    process(clk, reset)
    begin
        if reset = '1' then
            desc_reg <= (others => '0');
            bit_in_slot <= 0;
        elsif rising_edge(clk) then
            bclk_reg <= bclk_reg(1 downto 0) & tdm_bclk;
            wclk_reg <= wclk_reg(1 downto 0) & tdm_wclk;

            -- Frame-Sync am Anfang des TDM-Frames
            if wclk_reg(2 downto 1) = "01" then 
                bit_in_slot <= 0;
                desc_reg <= (others => '0'); -- Reset laut AES50 Standard
                ones_count <= 0;
            end if;

            -- Verarbeitung bei BCLK steigender Flanke
            if bclk_reg(2 downto 1) = "01" then
                
                -- Bit-Zähler innerhalb des 32-Bit Slots
                if bit_in_slot = 31 then
                    bit_in_slot <= 0;
                else
                    bit_in_slot <= bit_in_slot + 1;
                end if;

                -- Laut Sourcecode: Die ersten 16 Bits (31 bis 16) sind aux_i
                -- Da MSB zuerst kommt, sind das die Zählerstände 0 bis 15
                if bit_in_slot < 16 then
                    
                    -- A: De-Stuffing
                    if ones_count = 8 and tdm_aux_i = '0' then
                        valid_out <= '0';
                        ones_count <= 0;
                    else
                        valid_out <= '1';
                        
                        if tdm_aux_i = '1' then
                            ones_count <= ones_count + 1;
                        else
                            ones_count <= 0;
                        end if;

                        -- B: Descrambler
                        data_out <= tdm_aux_i xor desc_reg(3) xor desc_reg(8);
                        desc_reg <= desc_reg(7 downto 0) & tdm_aux_i;
                    end if;
                else
                    -- Bits 16-31 sind der interne Counter/Header -> Ignorieren
                    valid_out <= '0';
                    -- Wir setzen ones_count nicht zurück, um Stuffing über Wortgrenzen zu erlauben
                end if;
            end if;
        end if;
    end process;
end architecture;
