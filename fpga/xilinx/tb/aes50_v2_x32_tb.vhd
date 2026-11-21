-- ===========================================================================
-- OpenX32 AES50v2 Integration Testbench
--
-- Verifies OpenX32's audioclk + tdm_multichan_tx + tdm_multichan_rx +
-- aes50_consts + aes50_rst + aes50_v2_wrapper (G_FSYNC_DELAY = 1)
-- against a peer AES50v2 slave core (RMII + LVDS clock + UART loopback).
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_v2_x32_tb is
end entity;

architecture sim of aes50_v2_x32_tb is
	constant C_SIM_ASSM : natural := 64; -- shortened ASSM period for fast sim

	signal clk50        : std_logic := '0';
	signal clk_49m152   : std_logic := '0';
	signal online       : std_logic := '0';
	signal aes_rst      : std_logic;

	-- OpenX32 audioclk signals
	signal clk_24m576   : std_logic;
	signal bclk         : std_logic;
	signal fsync        : std_logic;

	-- OpenX32 aes50_consts signals
	signal c_dbg_len    : std_logic_vector(19 downto 0);
	signal c_ftx_48k    : std_logic_vector(22 downto 0);
	signal c_ftx_44k1   : std_logic_vector(22 downto 0);
	signal c_wd_clk     : std_logic_vector(5 downto 0);
	signal c_wd_rxdv    : std_logic_vector(14 downto 0);
	signal c_mdix_1ms   : std_logic_vector(16 downto 0);
	signal c_clk_ok_ref : std_logic_vector(19 downto 0);
	signal c_mult_48k   : std_logic_vector(31 downto 0);
	signal c_mult_44k1  : std_logic_vector(31 downto 0);
	signal c_uart_bit   : std_logic_vector(9 downto 0);
	signal c_uart_to    : std_logic_vector(19 downto 0);

	-- Shortened clk_ok_ref for simulation (500 ticks instead of 500000)
	signal sim_clk_ok   : std_logic_vector(19 downto 0) := std_logic_vector(to_unsigned(500, 20));
	signal sim_mdix_1ms : std_logic_vector(16 downto 0) := std_logic_vector(to_unsigned(500, 17));
	signal sim_uart_to  : std_logic_vector(19 downto 0) := std_logic_vector(to_unsigned(4000, 20));

	-- X32 TDM8 (6 audio lines = 48 channels)
	signal x32_tx_data  : std_logic_vector(6 * 24 - 1 downto 0) := (others => '0');
	signal x32_rx_data  : std_logic_vector(6 * 24 - 1 downto 0);
	signal x32_tdm_to_aes   : std_logic_vector(6 downto 0) := (others => '0');
	signal x32_tdm_from_aes : std_logic_vector(6 downto 0);

	-- RMII & LVDS clock interconnect between X32 (Master) and Peer (Slave)
	signal m_tx_en, s_tx_en : std_logic;
	signal m_txd,   s_txd   : std_logic_vector(1 downto 0);
	signal m_clka_tx, m_clka_en, m_clkb_tx, m_clkb_en : std_logic;
	signal s_clka_tx, s_clka_en, s_clkb_tx, s_clkb_en : std_logic;
	signal wire_clka, wire_clkb : std_logic;

	-- Peer TDM (looped back)
	signal s_tdm_loop   : std_logic_vector(6 downto 0);
	signal s_bclk_out   : std_logic;
	signal s_wclk_out   : std_logic;

	-- Status & UART
	signal m_aes_ok, s_aes_ok : std_logic;
	signal m_uart_tx, m_uart_rx : std_logic := '1';
	signal s_uart_loop : std_logic;
	signal m_phy_rst_n, s_phy_rst_n : std_logic;

	-- Slot counter on X32 TDM to drive unique sample per (line, slot)
	signal slot_idx     : natural range 0 to 7 := 0;
	signal bit_cnt_tb   : natural range 0 to 31 := 0;
	signal zfsync_tb    : std_logic := '0';

	function expected_sample(line_idx : natural; slot_num : natural) return std_logic_vector is
	begin
		-- Unique non-zero 24-bit pattern: 0xA5, line(3:0) & slot(3:0), 0x3C
		return x"A5" & std_logic_vector(to_unsigned(line_idx, 4)) & std_logic_vector(to_unsigned(slot_num, 4)) & x"3C";
	end function;

begin
	-- 50 MHz RMII clock (20 ns period)
	clk50 <= not clk50 after 10 ns;
	-- 49.152 MHz audio PLL clock (~20.345 ns period)
	clk_49m152 <= not clk_49m152 after 10.1725 ns;

	-- OpenX32 audioclk
	u_audioclk : entity work.audioclk
		port map (
			fs_x_1024_i => clk_49m152,
			fs_x_512_o  => clk_24m576,
			fs_x_256_o  => bclk,
			fs_x_128_o  => open,
			fs_x_64_o   => open,
			fs_o        => fsync
		);

	-- OpenX32 aes50_consts (verifies 50 MHz values!)
	u_consts : entity work.aes50_consts
		port map (
			debug_out_signal_pulse_len        => c_dbg_len,
			first_transmit_start_counter_48k  => c_ftx_48k,
			first_transmit_start_counter_44k1 => c_ftx_44k1,
			wd_aes_clk_timeout                => c_wd_clk,
			wd_aes_rx_dv_timeout              => c_wd_rxdv,
			mdix_timer_1ms_reference          => c_mdix_1ms,
			aes_clk_ok_counter_reference      => c_clk_ok_ref,
			mult_clk625_48k                   => c_mult_48k,
			mult_clk625_44k1                  => c_mult_44k1,
			uart_clks_per_bit                 => c_uart_bit,
			uart_timeout_clks                 => c_uart_to
		);

	-- OpenX32 aes50_rst on 50 MHz
	u_rst : entity work.aes50_rst
		port map (
			clk50_i => clk50,
			start_i => online,
			rst_o   => aes_rst
		);

	-- OpenX32 tdm_multichan_tx (6 lines -> AES50A)
	u_tdm_tx : entity work.tdm_multichan_tx
		generic map (
			TDM_RECEIVER => 6,
			DATA_WIDTH   => 24
		)
		port map (
			bclk    => bclk,
			fsync   => fsync,
			data_in => x32_tx_data,
			tdm_out => x32_tdm_to_aes(5 downto 0)
		);
	x32_tdm_to_aes(6) <= '0';

	-- OpenX32 tdm_multichan_rx (AES50A -> 6 lines)
	u_tdm_rx : entity work.tdm_multichan_rx
		generic map (
			TDM_RECEIVER => 6,
			DATA_WIDTH   => 24
		)
		port map (
			bclk     => bclk,
			fsync    => fsync,
			tdm_in   => x32_tdm_from_aes(5 downto 0),
			data_out => x32_rx_data
		);

	-- Drive unique 24-bit sample for each (line, slot) synchronized to tdm_multichan_tx
	process(bclk)
		variable next_slot : natural range 0 to 7;
	begin
		if rising_edge(bclk) then
			if (fsync = '1' and zfsync_tb = '0') then
				bit_cnt_tb <= 0;
				slot_idx   <= 0;
				next_slot  := 1;
			elsif bit_cnt_tb = 31 then
				bit_cnt_tb <= 0;
				if slot_idx = 7 then
					slot_idx <= 0;
				else
					slot_idx <= slot_idx + 1;
				end if;
				if slot_idx = 6 then
					next_slot := 0;
				elsif slot_idx = 7 then
					next_slot := 1;
				else
					next_slot := slot_idx + 2;
				end if;
			else
				bit_cnt_tb <= bit_cnt_tb + 1;
				next_slot := slot_idx;
			end if;
			zfsync_tb <= fsync;

			-- Prepare data_in for the NEXT slot before tdm_multichan_tx latches it at bit_cnt=31 / fsync rise
			if bit_cnt_tb = 15 then
				for l in 0 to 5 loop
					x32_tx_data((l * 24) + 23 downto l * 24) <= expected_sample(l, (slot_idx + 1) mod 8);
				end loop;
			end if;
		end if;
	end process;

	-- LVDS clock wires (crossed pairs: each side drives A and receives B when mdix=0)
	wire_clka <= m_clka_tx when m_clka_en = '1' else '0';
	wire_clkb <= s_clka_tx when s_clka_en = '1' else '0';

	-- DUT 1: OpenX32 AES50v2 wrapper (AES50 Master / TDM Slave, G_FSYNC_DELAY = 1)
	u_x32_aes50 : entity work.aes50_v2_wrapper
		generic map (
			G_SUPPORT_44K1     => false,
			G_UART_AUX         => true,
			G_TDM_AUX          => false,
			G_UART_FIFO_AW     => 10,
			G_FSYNC_DELAY      => 1,
			G_SIM_ASSM_SAMPLES => C_SIM_ASSM
		)
		port map (
			clk50_i                             => clk50,
			rst_i                               => aes_rst,
			fs_mode_i                           => "01", -- 48 kHz
			sys_mode_i                          => "10", -- AES50 Master, TDM Slave
			tdm8_i2s_mode_i                     => '0',  -- TDM8
			aux_tx_tdm_uart_select_i            => '1',  -- UART aux
			rmii_crs_dv_i                       => s_tx_en,
			rmii_rxd_i                          => s_txd,
			rmii_tx_en_o                        => m_tx_en,
			rmii_txd_o                          => m_txd,
			phy_rst_n_o                         => m_phy_rst_n,
			aes50_clk_a_rx_i                    => wire_clka,
			aes50_clk_a_tx_o                    => m_clka_tx,
			aes50_clk_a_tx_en_o                 => m_clka_en,
			aes50_clk_b_rx_i                    => wire_clkb,
			aes50_clk_b_tx_o                    => m_clkb_tx,
			aes50_clk_b_tx_en_o                 => m_clkb_en,
			clk_1024xfs_from_pll_i              => clk_49m152,
			pll_lock_n_i                        => '0',
			clk_to_pll_o                        => open,
			pll_mult_value_o                    => open,
			pll_init_busy_i                     => '0',
			mclk_o                              => open,
			wclk_o                              => open,
			bclk_o                              => open,
			wclk_readback_i                     => fsync,
			bclk_readback_i                     => bclk,
			wclk_out_en_o                       => open,
			bclk_out_en_o                       => open,
			tdm_i                               => x32_tdm_to_aes,
			tdm_o                               => x32_tdm_from_aes,
			i2s_i                               => '0',
			i2s_o                               => open,
			aes_ok_o                            => m_aes_ok,
			dbg_o                               => open,
			uart_o                              => m_uart_rx,
			uart_i                              => m_uart_tx,
			uart_overflow_o                     => open,
			debug_out_signal_pulse_len_i        => c_dbg_len,
			first_transmit_start_counter_48k_i  => c_ftx_48k,
			first_transmit_start_counter_44k1_i => c_ftx_44k1,
			wd_aes_clk_timeout_i                => c_wd_clk,
			wd_aes_rx_dv_timeout_i              => c_wd_rxdv,
			mdix_timer_1ms_reference_i          => sim_mdix_1ms,
			aes_clk_ok_counter_reference_i      => sim_clk_ok,
			mult_clk625_48k_i                   => c_mult_48k,
			mult_clk625_44k1_i                  => c_mult_44k1,
			uart_clks_per_bit_i                 => c_uart_bit,
			uart_timeout_clks_i                 => sim_uart_to
		);

	-- DUT 2: Stagebox peer (AES50 Slave / TDM Master, G_FSYNC_DELAY = 0, TDM & UART looped back)
	u_peer_aes50 : entity work.aes50_v2_wrapper
		generic map (
			G_SUPPORT_44K1     => false,
			G_UART_AUX         => true,
			G_TDM_AUX          => false,
			G_UART_FIFO_AW     => 10,
			G_FSYNC_DELAY      => 0,
			G_SIM_ASSM_SAMPLES => C_SIM_ASSM
		)
		port map (
			clk50_i                             => clk50,
			rst_i                               => aes_rst,
			fs_mode_i                           => "01",
			sys_mode_i                          => "00", -- AES50 Slave, TDM Master
			tdm8_i2s_mode_i                     => '0',
			aux_tx_tdm_uart_select_i            => '1',
			rmii_crs_dv_i                       => m_tx_en,
			rmii_rxd_i                          => m_txd,
			rmii_tx_en_o                        => s_tx_en,
			rmii_txd_o                          => s_txd,
			phy_rst_n_o                         => s_phy_rst_n,
			aes50_clk_a_rx_i                    => wire_clkb,
			aes50_clk_a_tx_o                    => s_clka_tx,
			aes50_clk_a_tx_en_o                 => s_clka_en,
			aes50_clk_b_rx_i                    => wire_clka,
			aes50_clk_b_tx_o                    => s_clkb_tx,
			aes50_clk_b_tx_en_o                 => s_clkb_en,
			clk_1024xfs_from_pll_i              => clk_49m152,
			pll_lock_n_i                        => '0',
			clk_to_pll_o                        => open,
			pll_mult_value_o                    => open,
			pll_init_busy_i                     => '0',
			mclk_o                              => open,
			wclk_o                              => s_wclk_out,
			bclk_o                              => s_bclk_out,
			wclk_readback_i                     => s_wclk_out,
			bclk_readback_i                     => s_bclk_out,
			wclk_out_en_o                       => open,
			bclk_out_en_o                       => open,
			tdm_i                               => s_tdm_loop,
			tdm_o                               => s_tdm_loop,
			i2s_i                               => '0',
			i2s_o                               => open,
			aes_ok_o                            => s_aes_ok,
			dbg_o                               => open,
			uart_o                              => s_uart_loop,
			uart_i                              => s_uart_loop,
			uart_overflow_o                     => open,
			debug_out_signal_pulse_len_i        => c_dbg_len,
			first_transmit_start_counter_48k_i  => c_ftx_48k,
			first_transmit_start_counter_44k1_i => c_ftx_44k1,
			wd_aes_clk_timeout_i                => c_wd_clk,
			wd_aes_rx_dv_timeout_i              => c_wd_rxdv,
			mdix_timer_1ms_reference_i          => sim_mdix_1ms,
			aes_clk_ok_counter_reference_i      => sim_clk_ok,
			mult_clk625_48k_i                   => c_mult_48k,
			mult_clk625_44k1_i                  => c_mult_44k1,
			uart_clks_per_bit_i                 => c_uart_bit,
			uart_timeout_clks_i                 => sim_uart_to
		);

	-- Main test process
	process
		variable matched_samples : natural := 0;
		variable rx_word         : std_logic_vector(23 downto 0);
		variable exp_word        : std_logic_vector(23 downto 0);
		variable rx_slot         : natural;
		variable uart_rx_byte    : std_logic_vector(7 downto 0);
		constant C_TEST_BYTE     : std_logic_vector(7 downto 0) := x"5A";
		constant C_BIT_TIME      : time := 434 * 20 ns; -- 115200 baud @ 50 MHz
	begin
		-- 1. Check aes50_consts values are properly halved for 50 MHz
		wait for 100 ns;
		assert to_integer(unsigned(c_uart_bit)) = 434
			report "FAIL: uart_clks_per_bit should be 434 for 115200@50MHz" severity failure;
		assert to_integer(unsigned(c_wd_clk)) = 25
			report "FAIL: wd_aes_clk_timeout should be 25 for 50MHz" severity failure;
		assert to_integer(unsigned(c_wd_rxdv)) = 7500
			report "FAIL: wd_aes_rx_dv_timeout should be 7500 for 50MHz" severity failure;
		assert to_integer(unsigned(c_mdix_1ms)) = 50000
			report "FAIL: mdix_timer_1ms_reference should be 50000 for 50MHz" severity failure;

		-- 2. Bring system online (deasserts aes_rst via aes50_rst on clk50)
		online <= '1';
		wait until m_aes_ok = '1' and s_aes_ok = '1' for 5 ms;
		assert m_aes_ok = '1' and s_aes_ok = '1'
			report "FAIL: AES50 link did not come up (m_aes_ok=" & std_logic'image(m_aes_ok) &
			       " s_aes_ok=" & std_logic'image(s_aes_ok) & ")" severity failure;
		report "INFO: AES50v2 link established (Master + Slave aes_ok_o = '1')";

		-- 3. Wait for sys_mode="10" WCLK alignment (1023 WCLK edges = 21.3 ms) + ASSM alignment (~3 ms)
		wait until x32_rx_data(23 downto 0) /= x"000000" for 35 ms;
		assert x32_rx_data(23 downto 0) /= x"000000"
			report "FAIL: No audio arrived back on X32 TDM RX within 35 ms" severity failure;
		wait until rising_edge(fsync);
		wait until rising_edge(fsync);
		for check_frames in 0 to 1 loop
			for s in 0 to 7 loop
				wait until rising_edge(bclk) and bit_cnt_tb = 10;
				-- tdm_multichan_rx latches previous slot at bit_cnt = 0
				rx_slot := (slot_idx + 7) mod 8;
				for l in 0 to 5 loop
					rx_word  := x32_rx_data((l * 24) + 23 downto l * 24);
					exp_word := expected_sample(l, rx_slot);
					assert rx_word = exp_word
						report "FAIL: TDM mismatch on line " & integer'image(l) &
						       " slot " & integer'image(rx_slot) &
						       " got 0x" & to_hstring(rx_word) &
						       " expected 0x" & to_hstring(exp_word)
						severity failure;
					matched_samples := matched_samples + 1;
				end loop;
			end loop;
		end loop;
		report "INFO: All " & integer'image(matched_samples) & " audio samples matched bit-exact across 6 TDM8 lines (48 ch) with G_FSYNC_DELAY=1!";

		-- 4. Send a UART byte at 115200 baud and verify it loops back through peer!
		m_uart_tx <= '0'; -- start bit
		wait for C_BIT_TIME;
		for b in 0 to 7 loop
			m_uart_tx <= C_TEST_BYTE(b);
			wait for C_BIT_TIME;
		end loop;
		m_uart_tx <= '1'; -- stop bit
		wait for C_BIT_TIME;

		-- Receive looped-back UART byte
		wait until m_uart_rx = '0' for 3 ms;
		assert m_uart_rx = '0' report "FAIL: Did not receive looped-back UART start bit" severity failure;
		wait for C_BIT_TIME / 2;
		for b in 0 to 7 loop
			wait for C_BIT_TIME;
			uart_rx_byte(b) := m_uart_rx;
		end loop;
		assert uart_rx_byte = C_TEST_BYTE
			report "FAIL: UART loopback mismatch got 0x" & to_hstring(uart_rx_byte) &
			       " expected 0x" & to_hstring(C_TEST_BYTE) severity failure;
		report "INFO: UART aux loopback matched bit-exact (0x5A @ 115200 baud / 50 MHz)!";

		report "PASS aes50_v2_x32_tb: 48ch TDM8 audio + 115200 baud UART aux + 50 MHz constants verified bit-exact";
		std.env.stop;
		wait;
	end process;
end architecture;
