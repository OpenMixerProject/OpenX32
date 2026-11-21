-- ===========================================================================
-- OpenX32 Testbench: aes50_par_bridge (48 kHz 48ch + SPI Aux + 96 kHz SRC)
--
-- Two aes50_par_bridge instances connected back-to-back (Master <-> Slave):
--   1. 48 kHz mode: verifies all 48 TDM8 channels arrive bit-exact on the
--      exact channel index (catches any RX_PLAY48 off-by-one shift).
--   2. SPI Aux tunnel: sends 16-byte message Master -> Slave via aux_tx_we_i
--      and checks every byte on Slave aux_rx_byte_o (catches aux_drain_on
--      duplicate word 0 / dropped word 43).
--   3. 96 kHz + SRC mode: verifies all 24 TDM8 channels arrive on the exact
--      matching channel index with expected steady-state DC amplitude (catches
--      TX_SEND96_P1 / RX_FEED96_P0 / RX_FEED96_P1 off-by-one shifts).
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_par_bridge_tb is
end entity;

architecture sim of aes50_par_bridge_tb is

	-- Clocks
	signal clk50       : std_logic := '0';
	signal clk_49m     : std_logic := '0'; -- 49.152 MHz (1024*fs)
	signal bclk        : std_logic := '0'; -- 12.288 MHz (256*fs)
	signal fsync       : std_logic := '0'; -- 48 kHz

	signal rst         : std_logic := '1';
	signal fs96        : std_logic := '0';

	-- RMII crossover
	signal m_tx_en, s_tx_en : std_logic;
	signal m_txd,   s_txd   : std_logic_vector(1 downto 0);

	-- LVDS clock crossover (mdix=0 drives A, receives B)
	signal m_clk_a_tx, m_clk_a_en, m_clk_b_tx, m_clk_b_en : std_logic;
	signal s_clk_a_tx, s_clk_a_en, s_clk_b_tx, s_clk_b_en : std_logic;
	signal m_clk_a_rx, m_clk_b_rx, s_clk_a_rx, s_clk_b_rx : std_logic;

	-- TDM8 audio (6 lines x 8 slots = 48 ch)
	signal m_tdm_i, m_tdm_o : std_logic_vector(5 downto 0) := (others => '0');
	signal s_tdm_i, s_tdm_o : std_logic_vector(5 downto 0) := (others => '0');

	-- SPI Aux ports
	signal m_aux_tx_we   : std_logic := '0';
	signal m_aux_tx_byte : std_logic_vector(7 downto 0) := (others => '0');
	signal s_aux_rx_rd   : std_logic := '0';
	signal s_aux_rx_byte : std_logic_vector(7 downto 0);
	signal s_aux_rx_val  : std_logic;
	signal s_aux_rx_cnt  : std_logic_vector(15 downto 0);

	-- Status
	signal m_aes_ok, m_rx_ok, m_tx_st : std_logic;
	signal s_aes_ok, s_rx_ok, s_tx_st : std_logic;

	-- TDM test harness arrays
	type ch48_t is array (0 to 47) of std_logic_vector(23 downto 0);
	signal tx_pat  : ch48_t := (others => (others => '0'));
	signal rx_cap  : ch48_t := (others => (others => '0'));
	signal rx_frm  : natural := 0;

begin

	clk50   <= not clk50   after 10 ns;
	clk_49m <= not clk_49m after 10172 ps; -- ~49.152 MHz

	-- Derive 12.288 MHz BCLK and 48 kHz FSYNC from clk_49m
	process (clk_49m)
		variable div4  : unsigned(1 downto 0) := "00";
		variable bcnt  : unsigned(7 downto 0) := (others => '0');
	begin
		if rising_edge(clk_49m) then
			div4 := div4 + 1;
			if div4 = 0 then
				bclk <= '1';
				if bcnt = 0 then
					fsync <= '1';
				else
					fsync <= '0';
				end if;
				bcnt := bcnt + 1;
			elsif div4 = 2 then
				bclk <= '0';
			end if;
		end if;
	end process;

	-- LVDS clock cable (pair 1 = Master A <-> Slave B, pair 2 = Slave A <-> Master B;
	-- each LVDS transceiver's RX sees its own driven pair when tx_en='1', matching hardware)
	m_clk_a_rx <= m_clk_a_tx when m_clk_a_en = '1' else s_clk_b_tx when s_clk_b_en = '1' else '0';
	s_clk_b_rx <= m_clk_a_tx when m_clk_a_en = '1' else s_clk_b_tx when s_clk_b_en = '1' else '0';
	s_clk_a_rx <= s_clk_a_tx when s_clk_a_en = '1' else m_clk_b_tx when m_clk_b_en = '1' else '0';
	m_clk_b_rx <= s_clk_a_tx when s_clk_a_en = '1' else m_clk_b_tx when m_clk_b_en = '1' else '0';

	-- TDM8 TX generator (drives Master m_tdm_i with 1-BCLK FSYNC delay)
	-- and TDM8 RX capture (reads Slave s_tdm_o)
	process (bclk)
		variable wprev   : std_logic := '0';
		variable bpos    : unsigned(7 downto 0) := (others => '0');
		variable cpos    : unsigned(7 downto 0);
		variable sbit    : natural range 0 to 31;
		variable slot    : natural range 0 to 7;
		variable sh_in   : ch48_t := (others => (others => '0'));
		type sh6_t is array (0 to 5) of std_logic_vector(23 downto 0);
		variable rx_sh   : sh6_t := (others => (others => '0'));
	begin
		if falling_edge(bclk) then
			-- Drive MSB first on falling edge so DUT samples on rising edge
			sbit := to_integer(bpos(4 downto 0));
			slot := to_integer(bpos(7 downto 5));
			if sbit <= 23 then
				for l in 0 to 5 loop
					m_tdm_i(l) <= tx_pat(l * 8 + slot)(23 - sbit);
				end loop;
			else
				m_tdm_i <= (others => '0');
			end if;
		end if;
		if rising_edge(bclk) then
			if fsync = '1' and wprev = '0' then
				cpos := to_unsigned(255, 8);
				rx_frm <= rx_frm + 1;
			else
				cpos := bpos;
			end if;
			wprev := fsync;
			bpos  := cpos + 1;
			sbit  := to_integer(cpos(4 downto 0));
			slot  := to_integer(cpos(7 downto 5));

			if sbit <= 23 then
				for l in 0 to 5 loop
					rx_sh(l) := rx_sh(l)(22 downto 0) & s_tdm_o(l);
				end loop;
			elsif sbit = 24 then
				for l in 0 to 5 loop
					rx_cap(l * 8 + slot) <= rx_sh(l);
				end loop;
			end if;
		end if;
	end process;

	-- Master bridge (sys_mode = "01", short simulation timers)
	u_master : entity work.aes50_par_bridge
		generic map (
			G_ENABLE_SRC      => true,
			G_ENABLE_AUX      => true,
			G_AUX_FIFO_AW     => 8,
			G_SIM_ASSM_INIT   => 2,
			G_SIM_ASSM_FRAMES => 32,
			G_SIM_WD_CLK      => 25,
			G_SIM_WD_RX_DV    => 7500,
			G_SIM_MDIX_1MS    => 5000,
			G_SIM_CLK_OK_REF  => 2000
		)
		port map (
			clk50_i             => clk50,
			rst_i               => rst,
			clk_1024xfs_i       => clk_49m,
			bclk_i              => bclk,
			fsync_i             => fsync,
			fs96_i              => fs96,
			sys_mode_i          => "01",
			rmii_crs_dv_i       => s_tx_en,
			rmii_rxd_i          => s_txd,
			rmii_tx_en_o        => m_tx_en,
			rmii_txd_o          => m_txd,
			phy_rst_n_o         => open,
			aes50_clk_a_rx_i    => m_clk_a_rx,
			aes50_clk_a_tx_o    => m_clk_a_tx,
			aes50_clk_a_tx_en_o => m_clk_a_en,
			aes50_clk_b_rx_i    => m_clk_b_rx,
			aes50_clk_b_tx_o    => m_clk_b_tx,
			aes50_clk_b_tx_en_o => m_clk_b_en,
			pll_lock_n_i        => '0',
			clk_to_pll_o        => open,
			tdm_i               => m_tdm_i,
			tdm_o               => m_tdm_o,
			aux_tx_we_i         => m_aux_tx_we,
			aux_tx_byte_i       => m_aux_tx_byte,
			aux_tx_count_o      => open,
			aux_rx_rd_i         => '0',
			aux_rx_byte_o       => open,
			aux_rx_valid_o      => open,
			aux_rx_count_o      => open,
			aes_ok_o            => m_aes_ok,
			rx_ok_o             => m_rx_ok,
			tx_started_o        => m_tx_st,
			rx_block_drop_o     => open,
			fmt_valid_o         => open,
			fmt_o               => open
		);

	-- Slave bridge (sys_mode = "00")
	u_slave : entity work.aes50_par_bridge
		generic map (
			G_ENABLE_SRC      => true,
			G_ENABLE_AUX      => true,
			G_AUX_FIFO_AW     => 8,
			G_SIM_ASSM_INIT   => 2,
			G_SIM_ASSM_FRAMES => 32,
			G_SIM_WD_CLK      => 25,
			G_SIM_WD_RX_DV    => 7500,
			G_SIM_MDIX_1MS    => 5000,
			G_SIM_CLK_OK_REF  => 2000
		)
		port map (
			clk50_i             => clk50,
			rst_i               => rst,
			clk_1024xfs_i       => clk_49m,
			bclk_i              => bclk,
			fsync_i             => fsync,
			fs96_i              => fs96,
			sys_mode_i          => "00",
			rmii_crs_dv_i       => m_tx_en,
			rmii_rxd_i          => m_txd,
			rmii_tx_en_o        => s_tx_en,
			rmii_txd_o          => s_txd,
			phy_rst_n_o         => open,
			aes50_clk_a_rx_i    => s_clk_a_rx,
			aes50_clk_a_tx_o    => s_clk_a_tx,
			aes50_clk_a_tx_en_o => s_clk_a_en,
			aes50_clk_b_rx_i    => s_clk_b_rx,
			aes50_clk_b_tx_o    => s_clk_b_tx,
			aes50_clk_b_tx_en_o => s_clk_b_en,
			pll_lock_n_i        => '0',
			clk_to_pll_o        => open,
			tdm_i               => s_tdm_i,
			tdm_o               => s_tdm_o,
			aux_tx_we_i         => '0',
			aux_tx_byte_i       => x"00",
			aux_tx_count_o      => open,
			aux_rx_rd_i         => s_aux_rx_rd,
			aux_rx_byte_o       => s_aux_rx_byte,
			aux_rx_valid_o      => s_aux_rx_val,
			aux_rx_count_o      => s_aux_rx_cnt,
			aes_ok_o            => s_aes_ok,
			rx_ok_o             => s_rx_ok,
			tx_started_o        => s_tx_st,
			rx_block_drop_o     => open,
			fmt_valid_o         => open,
			fmt_o               => open
		);

	-- Stimulus & checks
	process
		constant C_MSG : string := "X32_AUX_TEST_01!";
		variable got   : string(1 to C_MSG'length);
		variable idx   : natural;
		variable exp_v : signed(23 downto 0);
		variable got_v : signed(23 downto 0);
		variable diff  : integer;
		variable errs  : natural := 0;
	begin
		-- ============================================================
		-- TEST 1: 48 kHz 48-channel bit-exact TDM8 + SPI Aux round-trip
		-- ============================================================
		fs96 <= '0';
		rst  <= '1';
		for ch in 0 to 47 loop
			tx_pat(ch) <= std_logic_vector(to_unsigned(16#100000# + ch * 16#001101#, 24));
		end loop;
		wait for 500 ns;
		rst <= '0';

		-- Wait for AES50 link lock
		wait until s_aes_ok = '1' and s_rx_ok = '1' for 8 ms;
		assert s_aes_ok = '1' and s_rx_ok = '1'
			report "FAIL: 48 kHz AES50 link did not lock" severity failure;

		-- Let playout ring + TDM8 settle for 20 frames (~420 us)
		wait for 500 us;

		for ch in 0 to 47 loop
			if rx_cap(ch) /= tx_pat(ch) then
				report "FAIL 48k ch " & integer'image(ch) &
					" got=" & integer'image(to_integer(unsigned(rx_cap(ch)))) &
					" exp=" & integer'image(to_integer(unsigned(tx_pat(ch))))
					severity error;
				errs := errs + 1;
			end if;
		end loop;
		assert errs = 0 report "FAIL: 48 kHz TDM8 channel mismatches" severity failure;
		report "PASS Test 1a: 48 kHz 48-channel TDM8 bit-exact on all 48 channels!";

		-- Send 16-byte SPI Aux message on Master (spaced by 4 us like real SPI)
		for i in 1 to C_MSG'length loop
			wait until rising_edge(clk50);
			m_aux_tx_we   <= '1';
			m_aux_tx_byte <= std_logic_vector(to_unsigned(character'pos(C_MSG(i)), 8));
			wait until rising_edge(clk50);
			m_aux_tx_we   <= '0';
			wait for 4 us;
		end loop;

		-- Receive 16 bytes on Slave SPI Aux RX FIFO
		idx := 0;
		for t in 1 to 6000 loop
			wait for 1 us;
			if s_aux_rx_val = '1' then
				idx := idx + 1;
				got(idx) := character'val(to_integer(unsigned(s_aux_rx_byte)));
				wait until rising_edge(clk50);
				s_aux_rx_rd <= '1';
				wait until rising_edge(clk50);
				s_aux_rx_rd <= '0';
				wait until rising_edge(clk50);
				exit when idx = C_MSG'length;
			end if;
		end loop;
		assert idx = C_MSG'length and got = C_MSG
			report "FAIL Test 1b: SPI Aux got " & integer'image(idx) & " bytes: '" & got &
				"' byte6=" & integer'image(character'pos(got(6)))
			severity failure;
		report "PASS Test 1b: SPI Aux 16-byte message '" & got & "' received bit-exact!";

		-- ============================================================
		-- TEST 2: 96 kHz 24-channel + Half-Band FIR SRC
		-- ============================================================
		rst  <= '1';
		fs96 <= '1';
		for ch in 0 to 23 loop
			-- Distinct signed DC value per channel: (ch+1) * 4096
			tx_pat(ch) <= std_logic_vector(to_signed((ch + 1) * 4096, 24));
		end loop;
		for ch in 24 to 47 loop
			tx_pat(ch) <= (others => '0');
		end loop;
		wait for 1 us;
		rst <= '0';

		wait until s_aes_ok = '1' and s_rx_ok = '1' for 8 ms;
		assert s_aes_ok = '1' and s_rx_ok = '1'
			report "FAIL: 96 kHz AES50 link did not lock" severity failure;

		-- Wait > 64 frames (1.5 ms) so both 47-tap TX & RX half-band FIRs settle
		wait for 2 ms;

		for ch in 0 to 23 loop
			exp_v := to_signed((ch + 1) * 4096, 24);
			got_v := signed(rx_cap(ch));
			diff  := abs(to_integer(got_v) - to_integer(exp_v));
			if diff > 16 then
				report "FAIL 96k+SRC ch " & integer'image(ch) &
					" got=" & integer'image(to_integer(got_v)) &
					" exp=" & integer'image(to_integer(exp_v))
					severity error;
				errs := errs + 1;
			end if;
		end loop;
		assert errs = 0 report "FAIL: 96 kHz + SRC channel mismatches" severity failure;
		report "PASS Test 2: 96 kHz 24-channel + SRC exact channel mapping & unity gain!";

		report "ALL aes50_par_bridge_tb TESTS PASSED!";
		std.env.stop;
	end process;

end architecture sim;
