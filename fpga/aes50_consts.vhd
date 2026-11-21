
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_consts is
	port (
		debug_out_signal_pulse_len				: out natural range 0 to 1000000;
		first_transmit_start_counter_48k		: out natural range 0 to 5000000;
		first_transmit_start_counter_44k1	: out natural range 0 to 5000000;
		
		wd_aes_clk_timeout						: out natural range 0 to 50;
		wd_aes_rx_dv_timeout			   		: out natural range 0 to 20000;
		mdix_timer_1ms_reference				: out natural range 0 to 100000;
		aes_clk_ok_counter_reference			: out natural range 0 to 1000000;
		--Those are the multiplicators needed if we are tdm-master as well as aes-master -> we feed the PLL with a 6.25 MHz clock generated through our 50 MHz clock
		mult_clk625_48k							: out natural;
		mult_clk625_44k1							: out natural;
		uart_clks_per_bit							: out natural range 0 to 65535;
		uart_timeout_clks							: out natural range 0 to 16777215	
	);
end entity;

architecture behavioral of aes50_consts is
begin
	-- settings for 50 MHz
	debug_out_signal_pulse_len <= 500000;
	first_transmit_start_counter_48k <= 0; -- not used in AES50v2
	first_transmit_start_counter_44k1 <= 0; -- not used in AES50v2
	wd_aes_clk_timeout <= 25;
	wd_aes_rx_dv_timeout <= 7500;
	mdix_timer_1ms_reference <= 50000;
	aes_clk_ok_counter_reference <= 500000;
	-- PLL multiplicators for the 6.25 MHz reference (now 50 MHz / 8), unchanged
	mult_clk625_48k <= 8246337;
	mult_clk625_44k1 <= 7576322;
	uart_clks_per_bit <= 434; -- 115200 baud
	uart_timeout_clks <= 500000; -- 10 ms

	
	-- the following settings are for AES50v1 that previously used a clock between 75 and 100 MHz
	
	-- settings for 100 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(1000000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(4249500, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(4610800, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(50, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(15000, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(100000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(1000000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(8246337, 31));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(7576322, 31));
	--uart_clks_per_bit <= std_logic_vector(to_unsigned(868, 16));
	--uart_timeout_clks <= std_logic_vector(to_unsigned(1000000, 24));
	
	-- settings for 90 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(900000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3824550, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(4149720, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(45, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(13500, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(90000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(900000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(7421703, 31));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(6818670, 31));
	--uart_clks_per_bit <= std_logic_vector(to_unsigned(781, 16));
	--uart_timeout_clks <= std_logic_vector(to_unsigned(900000, 24));

	-- settings for 80 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(800000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3399600, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(3688640, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(40, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(12000, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(80000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(800000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(6597070, 31));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(6061058, 31));
	--uart_clks_per_bit <= std_logic_vector(to_unsigned(694, 16));
	--uart_timeout_clks <= std_logic_vector(to_unsigned(800000, 24));
	
	-- settings for 75 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(750000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3187125, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(3458100, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(38, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(11250, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(75000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(750000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(6184753, 31));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(5682242, 31));
	--uart_clks_per_bit <= std_logic_vector(to_unsigned(651, 16));
	--uart_timeout_clks <= std_logic_vector(to_unsigned(750000, 24));
end behavioral;