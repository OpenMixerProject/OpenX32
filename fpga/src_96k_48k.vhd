-- ===========================================================================
-- OpenX32 - Synchronous 2:1 Half-Band FIR Sample Rate Converter (96 <-> 48 kHz)
--
-- 24-channel bidirectional synchronous SRC for 96 kHz AES50 stageboxes
-- (e.g. Midas DL251 / DL15x / DL231) locked to the X32 48 kHz master clock.
--
-- Filter: 47-tap Kaiser-windowed (beta = 8.5) symmetric half-band FIR, Q1.17
--   * Passband (0..18 kHz): +/- 0.0005 dB; 20 kHz: -0.12 dB
--   * Stopband (30..48 kHz): -85 to -99 dB
--   * Center tap = 0.5 exact (right-shift by 1, zero multiplier)
--   * 12 symmetric odd-tap pairs -> 12 MACs per channel per direction
--   * Latency: ~23 samples @ 96 kHz (~0.24 ms)
--
-- Clocking: Time-multiplexed on clk50_i (50 MHz).
--   * 24 ch * 12 taps * 2 reads = ~600 clocks / direction (fits easily in
--     1041 clocks per 48 kHz sample period).
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity src_96k_48k is
	generic (
		G_NUM_CH : natural := 24
	);
	port (
		clk_i           : in  std_logic; -- 50 MHz
		rst_i           : in  std_logic;

		-- ------------------------------------------------------------
		-- RX Decimator (96 kHz -> 48 kHz): 24 channels
		-- Write phase 0 (even) and phase 1 (odd) samples at 96 kHz;
		-- pulse rx_run_i once per 48 kHz period after phase 1 is written.
		-- ------------------------------------------------------------
		rx_we_i         : in  std_logic;
		rx_phase_i      : in  std_logic; -- '0' = even (first half), '1' = odd (second half)
		rx_ch_i         : in  unsigned(4 downto 0); -- 0..23
		rx_sample_i     : in  std_logic_vector(23 downto 0);
		rx_run_i        : in  std_logic; -- 1-clock pulse after phase 1 writes complete

		rx_out_valid_o  : out std_logic;
		rx_out_ch_o     : out unsigned(4 downto 0);
		rx_out_sample_o : out std_logic_vector(23 downto 0);

		-- ------------------------------------------------------------
		-- TX Interpolator (48 kHz -> 96 kHz): 24 channels
		-- Write one 48 kHz sample per channel, then pulse tx_run_i;
		-- outputs phase 0 (center tap) and phase 1 (interpolated) for each ch.
		-- ------------------------------------------------------------
		tx_we_i         : in  std_logic;
		tx_ch_i         : in  unsigned(4 downto 0); -- 0..23
		tx_sample_i     : in  std_logic_vector(23 downto 0);
		tx_run_i        : in  std_logic;

		tx_out_valid_o  : out std_logic;
		tx_out_phase_o  : out std_logic; -- '0' = first 96k sample, '1' = second 96k sample
		tx_out_ch_o     : out unsigned(4 downto 0);
		tx_out_sample_o : out std_logic_vector(23 downto 0)
	);
end entity src_96k_48k;

architecture rtl of src_96k_48k is

	-- 47-tap half-band symmetric odd-tap coefficients in Q1.17 (1.0 = 131072)
	-- k=0 corresponds to offset +-1, k=11 corresponds to offset +-23
	type coeff_arr_t is array (0 to 11) of signed(17 downto 0);
	constant C_COEFF : coeff_arr_t := (
		0  => to_signed( 41408, 18),
		1  => to_signed(-12990, 18),
		2  => to_signed(  6895, 18),
		3  => to_signed( -4084, 18),
		4  => to_signed(  2457, 18),
		5  => to_signed( -1441, 18),
		6  => to_signed(   801, 18),
		7  => to_signed(  -411, 18),
		8  => to_signed(   188, 18),
		9  => to_signed(   -73, 18),
		10 => to_signed(    21, 18),
		11 => to_signed(    -3, 18)
	);

	-- Saturate 25+ bit signed value to 24-bit signed std_logic_vector
	function sat24(v : signed) return std_logic_vector is
		constant MAX24 : signed(23 downto 0) := to_signed( 8388607, 24);
		constant MIN24 : signed(23 downto 0) := to_signed(-8388608, 24);
	begin
		if v > resize(MAX24, v'length) then
			return std_logic_vector(MAX24);
		elsif v < resize(MIN24, v'length) then
			return std_logic_vector(MIN24);
		else
			return std_logic_vector(resize(v, 24));
		end if;
	end function;

	-- ================================================================
	-- RX Decimator (96 -> 48 kHz)
	--   odd_ram  : 32 slots/ch (ch[4:0] & slot[4:0]) -> 1024 x 24 bit
	--   even_ram : 16 slots/ch (ch[4:0] & slot[3:0]) ->  512 x 24 bit
	-- ================================================================
	type ram1024_t is array (0 to 1023) of std_logic_vector(23 downto 0);
	type ram512_t  is array (0 to  511) of std_logic_vector(23 downto 0);

	signal rx_odd_ram   : ram1024_t := (others => (others => '0'));
	signal rx_even_ram  : ram512_t  := (others => (others => '0'));
	signal rx_odd_rd    : std_logic_vector(23 downto 0) := (others => '0');
	signal rx_even_rd   : std_logic_vector(23 downto 0) := (others => '0');

	signal rx_odd_ptr   : unsigned(4 downto 0) := (others => '0');
	signal rx_even_ptr  : unsigned(3 downto 0) := (others => '0');

	type dec_st_t is (D_IDLE, D_RD_A, D_RD_B, D_MAC, D_DONE);
	signal dec_st       : dec_st_t := D_IDLE;
	signal dec_ch       : unsigned(4 downto 0) := (others => '0');
	signal dec_k        : unsigned(3 downto 0) := (others => '0');
	signal dec_samp_a   : signed(23 downto 0) := (others => '0');
	signal dec_center   : signed(23 downto 0) := (others => '0');
	signal dec_acc      : signed(47 downto 0) := (others => '0');
	signal rx_odd_raddr : unsigned(9 downto 0) := (others => '0');
	signal rx_even_raddr: unsigned(8 downto 0) := (others => '0');

	-- ================================================================
	-- TX Interpolator (48 -> 96 kHz)
	--   tx_ram   : 32 slots/ch (ch[4:0] & slot[4:0]) -> 1024 x 24 bit
	-- ================================================================
	signal tx_ram       : ram1024_t := (others => (others => '0'));
	signal tx_rd        : std_logic_vector(23 downto 0) := (others => '0');
	signal tx_ptr       : unsigned(4 downto 0) := (others => '0');
	signal tx_raddr     : unsigned(9 downto 0) := (others => '0');

	type int_st_t is (I_IDLE, I_CENTER_RD, I_CENTER_OUT, I_RD_A, I_RD_B, I_MAC, I_DONE);
	signal int_st       : int_st_t := I_IDLE;
	signal int_ch       : unsigned(4 downto 0) := (others => '0');
	signal int_k        : unsigned(3 downto 0) := (others => '0');
	signal int_samp_a   : signed(23 downto 0) := (others => '0');
	signal int_acc      : signed(47 downto 0) := (others => '0');

begin

	-- ================================================================
	-- RX RAM write/read (simple dual-port BRAM inference)
	-- ================================================================
	process (clk_i)
		variable waddr_odd  : unsigned(9 downto 0);
		variable waddr_even : unsigned(8 downto 0);
	begin
		if rising_edge(clk_i) then
			if rx_we_i = '1' and rx_ch_i < G_NUM_CH then
				if rx_phase_i = '1' then
					waddr_odd := rx_ch_i & rx_odd_ptr;
					rx_odd_ram(to_integer(waddr_odd)) <= rx_sample_i;
				else
					waddr_even := rx_ch_i & rx_even_ptr;
					rx_even_ram(to_integer(waddr_even)) <= rx_sample_i;
				end if;
			end if;
			rx_odd_rd  <= rx_odd_ram(to_integer(rx_odd_raddr));
			rx_even_rd <= rx_even_ram(to_integer(rx_even_raddr));
		end if;
	end process;

	-- ================================================================
	-- RX Decimator FSM (47-tap half-band FIR, 96 -> 48 kHz)
	--   y[m] = 0.5 * even[m - 11] + sum_{k=0..11} c[k] * (odd[m - 11 + k] + odd[m - 12 - k])
	-- ================================================================
	process (clk_i)
		variable sum_pair : signed(24 downto 0);
		variable prod     : signed(42 downto 0);
		variable rounded  : signed(30 downto 0);
		variable idx_a    : unsigned(4 downto 0);
		variable idx_b    : unsigned(4 downto 0);
		variable idx_c    : unsigned(3 downto 0);
	begin
		if rising_edge(clk_i) then
			rx_out_valid_o <= '0';

			if rst_i = '1' then
				dec_st      <= D_IDLE;
				rx_odd_ptr  <= (others => '0');
				rx_even_ptr <= (others => '0');
			else
				case dec_st is
					when D_IDLE =>
						if rx_run_i = '1' then
							dec_ch <= (others => '0');
							dec_k  <= (others => '0');
							dec_acc <= (others => '0');
							-- Preload first read: odd[m - 11 + 0] and even[m - 11]
							idx_a := rx_odd_ptr - to_unsigned(11, 5);
							idx_c := rx_even_ptr - to_unsigned(11, 4);
							rx_odd_raddr  <= to_unsigned(0, 5) & idx_a;
							rx_even_raddr <= to_unsigned(0, 5) & idx_c;
							dec_st <= D_RD_A;
						end if;

					when D_RD_A =>
						-- Issue read for second half of symmetric pair: odd[m - 12 - k]
						idx_b := rx_odd_ptr - to_unsigned(12, 5) - resize(dec_k, 5);
						rx_odd_raddr <= dec_ch & idx_b;
						dec_st <= D_RD_B;

					when D_RD_B =>
						-- rx_odd_rd now holds samp_a; rx_even_rd holds center tap (on k=0)
						dec_samp_a <= signed(rx_odd_rd);
						if dec_k = 0 then
							dec_center <= signed(rx_even_rd);
						end if;
						-- Preload next tap's first read if more taps remain
						if dec_k < 11 then
							idx_a := rx_odd_ptr - to_unsigned(11, 5) + resize(dec_k + 1, 5);
							rx_odd_raddr <= dec_ch & idx_a;
						end if;
						dec_st <= D_MAC;

					when D_MAC =>
						-- rx_odd_rd now holds samp_b
						sum_pair := resize(dec_samp_a, 25) + resize(signed(rx_odd_rd), 25);
						prod     := sum_pair * C_COEFF(to_integer(dec_k));
						if dec_k = 0 then
							-- Initialize accumulator with 0.5 * center_tap (Q1.17 -> shift left 16) + rounding 0.5 LSB
							dec_acc <= shift_left(resize(dec_center, 48), 16) + to_signed(65536, 48) + resize(prod, 48);
						else
							dec_acc <= dec_acc + resize(prod, 48);
						end if;

						if dec_k = 11 then
							dec_st <= D_DONE;
						else
							dec_k  <= dec_k + 1;
							-- Next samp_a read was already issued in D_RD_B; now issue samp_b
							idx_b := rx_odd_ptr - to_unsigned(12, 5) - resize(dec_k + 1, 5);
							rx_odd_raddr <= dec_ch & idx_b;
							dec_st <= D_RD_B;
						end if;

					when D_DONE =>
						rounded := resize(shift_right(dec_acc, 17), 31);
						rx_out_valid_o  <= '1';
						rx_out_ch_o     <= dec_ch;
						rx_out_sample_o <= sat24(rounded);

						if dec_ch = G_NUM_CH - 1 then
							rx_odd_ptr  <= rx_odd_ptr + 1;
							rx_even_ptr <= rx_even_ptr + 1;
							dec_st      <= D_IDLE;
						else
							dec_ch <= dec_ch + 1;
							dec_k  <= (others => '0');
							idx_a  := rx_odd_ptr - to_unsigned(11, 5);
							idx_c  := rx_even_ptr - to_unsigned(11, 4);
							rx_odd_raddr  <= (dec_ch + 1) & idx_a;
							rx_even_raddr <= (dec_ch + 1) & idx_c;
							dec_st <= D_RD_A;
						end if;
				end case;
			end if;
		end if;
	end process;

	-- ================================================================
	-- TX RAM write/read (simple dual-port BRAM inference)
	-- ================================================================
	process (clk_i)
		variable waddr_tx : unsigned(9 downto 0);
	begin
		if rising_edge(clk_i) then
			if tx_we_i = '1' and tx_ch_i < G_NUM_CH then
				waddr_tx := tx_ch_i & tx_ptr;
				tx_ram(to_integer(waddr_tx)) <= tx_sample_i;
			end if;
			tx_rd <= tx_ram(to_integer(tx_raddr));
		end if;
	end process;

	-- ================================================================
	-- TX Interpolator FSM (47-tap half-band FIR, 48 -> 96 kHz)
	--   phase 0: x[m - 11]  (exact unity gain, zero compute)
	--   phase 1: 2 * sum_{k=0..11} c[k] * (x[m - 11 + k] + x[m - 12 - k])
	-- ================================================================
	process (clk_i)
		variable sum_pair : signed(24 downto 0);
		variable prod     : signed(42 downto 0);
		variable rounded  : signed(30 downto 0);
		variable idx_a    : unsigned(4 downto 0);
		variable idx_b    : unsigned(4 downto 0);
	begin
		if rising_edge(clk_i) then
			tx_out_valid_o <= '0';

			if rst_i = '1' then
				int_st <= I_IDLE;
				tx_ptr <= (others => '0');
			else
				case int_st is
					when I_IDLE =>
						if tx_run_i = '1' then
							int_ch   <= (others => '0');
							int_k    <= (others => '0');
							tx_raddr <= to_unsigned(0, 5) & (tx_ptr - to_unsigned(11, 5));
							int_st   <= I_CENTER_RD;
						end if;

					when I_CENTER_RD =>
						-- Wait 1 cycle for registered RAM read of x[m - 11], preload odd-pair A (k=0 -> m-11)
						tx_raddr <= int_ch & (tx_ptr - to_unsigned(11, 5));
						int_st   <= I_CENTER_OUT;

					when I_CENTER_OUT =>
						-- Emit phase 0 (exact delayed input)
						tx_out_valid_o  <= '1';
						tx_out_phase_o  <= '0';
						tx_out_ch_o     <= int_ch;
						tx_out_sample_o <= tx_rd;
						-- Now issue read for B (k=0 -> m-12)
						int_k    <= (others => '0');
						tx_raddr <= int_ch & (tx_ptr - to_unsigned(12, 5));
						int_st   <= I_RD_B;

					when I_RD_A =>
						idx_b    := tx_ptr - to_unsigned(12, 5) - resize(int_k, 5);
						tx_raddr <= int_ch & idx_b;
						int_st   <= I_RD_B;

					when I_RD_B =>
						int_samp_a <= signed(tx_rd);
						if int_k < 11 then
							idx_a    := tx_ptr - to_unsigned(11, 5) + resize(int_k + 1, 5);
							tx_raddr <= int_ch & idx_a;
						end if;
						int_st <= I_MAC;

					when I_MAC =>
						sum_pair := resize(int_samp_a, 25) + resize(signed(tx_rd), 25);
						prod     := sum_pair * C_COEFF(to_integer(int_k));
						if int_k = 0 then
							-- Factor of 2 gain for 2x interpolation -> divide by 2^16 instead of 2^17 (rounding = 32768)
							int_acc <= to_signed(32768, 48) + resize(prod, 48);
						else
							int_acc <= int_acc + resize(prod, 48);
						end if;

						if int_k = 11 then
							int_st <= I_DONE;
						else
							int_k    <= int_k + 1;
							idx_b    := tx_ptr - to_unsigned(12, 5) - resize(int_k + 1, 5);
							tx_raddr <= int_ch & idx_b;
							int_st   <= I_RD_B;
						end if;

					when I_DONE =>
						-- Shift right by 16 (2x interpolation gain on Q1.17 coefficients)
						rounded := resize(shift_right(int_acc, 16), 31);
						tx_out_valid_o  <= '1';
						tx_out_phase_o  <= '1';
						tx_out_ch_o     <= int_ch;
						tx_out_sample_o <= sat24(rounded);

						if int_ch = G_NUM_CH - 1 then
							tx_ptr <= tx_ptr + 1;
							int_st <= I_IDLE;
						else
							int_ch   <= int_ch + 1;
							tx_raddr <= (int_ch + 1) & (tx_ptr - to_unsigned(11, 5));
							int_st   <= I_CENTER_RD;
						end if;
				end case;
			end if;
		end if;
	end process;

end architecture rtl;
