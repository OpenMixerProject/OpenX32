-- VHDL File to route multiple audio-inputs to multiple audio-outputs
-- v0.0.3, 05.08.2025
-- OpenX32 Project
-- https://github.com/xn--nding-jua/OpenX32

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity audiomatrix is
	generic (
		DATA_WIDTH			: integer := 24;	-- 24-bit audio-samples
		NUM_INPUT_PORTS	: integer := 112;	-- 32x Analog-Input, 32x Card-Input, 8x AUX-Input, 40 DSP-Output
		NUM_OUTPUT_PORTS	: integer := 112	-- 16x Analog-Ouput, 32x Card-Output, 8x AUX-Output, 16x UltraNet-Output, 40x DSP-Input
	);
	port (
		clk					: in std_logic;
		sync_in				: in std_logic;
		input_data			: in  std_logic_vector(NUM_INPUT_PORTS * DATA_WIDTH - 1 downto 0);
		select_lines		: in  std_logic_vector(NUM_OUTPUT_PORTS * 8 - 1 downto 0); -- log2(32) = 5, log2(72) = 7, log2(112) = 7 bit
		i_ram_data			: in  std_logic_vector(DATA_WIDTH - 1 downto 0);
		
		output_data			: out std_logic_vector(NUM_OUTPUT_PORTS * DATA_WIDTH - 1 downto 0);
		o_ram_write_addr	: out unsigned(6 downto 0); -- log2(112) = 7 bit
		o_ram_read_addr	: out unsigned(6 downto 0); -- log2(112) = 7 bit
		o_ram_data			: out std_logic_vector(DATA_WIDTH - 1 downto 0);
		o_ram_wr_en			: out std_logic
	);
end entity audiomatrix;

architecture behavioral of audiomatrix is
	signal pRam				: integer range 0 to NUM_INPUT_PORTS - 1;
	signal pInput			: integer range 0 to NUM_INPUT_PORTS * DATA_WIDTH;
	signal pSelect			: integer range 0 to NUM_INPUT_PORTS * 8;
	signal pOutput			: integer range 0 to NUM_OUTPUT_PORTS * DATA_WIDTH;
	signal zsync			: std_logic;
begin
	blockram_ctrl : process(clk)
	begin
		if rising_edge(clk) then
			zsync <= sync_in;
		
			if (sync_in = '1' and zsync = '0') then
				-- Aktiviert den Schreibvorgang für den gesamten Vektor
				o_ram_write_addr <= to_unsigned(0, 7); -- write to first element
				o_ram_data <= input_data(DATA_WIDTH - 1 downto 0); -- first input-element
				o_ram_wr_en <= '1';

				pRam <= 1; -- preload to next write-address
				pInput <= DATA_WIDTH; -- preload to next write-address
			else
				-- Schreiben der restlichen Daten
				if pRam < NUM_INPUT_PORTS - 1 then
					o_ram_write_addr <= to_unsigned(pRam, 7);
					o_ram_data <= input_data(pInput + DATA_WIDTH - 1 downto pInput);
					o_ram_wr_en <= '1';

					-- increment for next write
					pRam <= pRam + 1;
					pInput <= pInput + DATA_WIDTH;
				else
					o_ram_wr_en <= '0';
				end if;
			end if;
		end if;
	end process;

	audiodata : process(clk)
		--variable i : integer range 0 to NUM_OUTPUT_PORTS - 1;
	begin
		if rising_edge(clk) then
			if (sync_in = '1' and zsync = '0') then
				-- set read-address for next read-operation
				o_ram_read_addr <= unsigned(select_lines(6 downto 0)); -- we are taking only 7-bit out of this 8-bit value

				pSelect <= 8; -- preload to next read-address
				pOutput <= 0; -- preload to first output-data
			else
				if (pSelect < NUM_INPUT_PORTS * 8) then
					-- read data
					output_data(pOutput + DATA_WIDTH - 1 downto pOutput) <= i_ram_data;
					
					-- set read-address for next read-operation
					o_ram_read_addr <= unsigned(select_lines(pSelect + 6 downto pSelect)); -- we are taking only 7-bit out of this 8-bit value
					
					-- increase pointers
					pSelect <= pSelect + 8;
					pOutput <= pOutput + DATA_WIDTH;
				end if;
			end if;
		end if;
	end process;
end architecture behavioral;