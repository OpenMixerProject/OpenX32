
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity aes50_consts is
	port (
		debug_out_signal_pulse_len				: out std_logic_vector(19 downto 0);		-- 1000000@100MHz
		first_transmit_start_counter_48k		: out std_logic_vector(22 downto 0);		-- 4249500@100MHz
		first_transmit_start_counter_44k1	: out std_logic_vector(22 downto 0);		-- 4610800@100MHz	
		
		wd_aes_clk_timeout						: out std_logic_vector(5 downto 0); 		-- 50@100MHz
		wd_aes_rx_dv_timeout			   		: out std_logic_vector(14 downto 0);		-- 15000@100MHz	
		mdix_timer_1ms_reference				: out std_logic_vector(16 downto 0);		-- 100000@100MHz
		aes_clk_ok_counter_reference			: out std_logic_vector(19 downto 0);		-- 1000000@100MHz
		--Those are the multiplicators needed if we are tdm-master as well as aes-master -> we feed the PLL with a 6.25 MHz clock generated through our 100 MHz clock-domain and multiply to get 49.152 or 45.1584...
		mult_clk625_48k							: out std_logic_vector(31 downto 0);		-- 8246337@100MHz
		mult_clk625_44k1							: out std_logic_vector(31 downto 0)			-- 7576322@100MHz
	);
end entity;

architecture behavioral of aes50_consts is
begin
	-- settings for 100 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(1000000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(4249500, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(4610800, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(50, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(15000, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(100000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(1000000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(8246337, 32));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(7576322, 32));
	
	-- settings for 90 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(900000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3824550, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(4149720, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(45, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(13500, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(90000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(900000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(7421703, 32));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(6818670, 32));

	-- settings for 80 MHz
	debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(800000, 20));
	first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3399600, 23));
	first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(3688640, 23));
	wd_aes_clk_timeout <= std_logic_vector(to_unsigned(40, 6));
	wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(12000, 15));
	mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(80000, 17));
	aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(800000, 20));
	mult_clk625_48k <= std_logic_vector(to_unsigned(6597070, 32));
	mult_clk625_44k1 <= std_logic_vector(to_unsigned(6061058, 32));
	
	-- settings for 75 MHz
	--debug_out_signal_pulse_len <= std_logic_vector(to_unsigned(750000, 20));
	--first_transmit_start_counter_48k <= std_logic_vector(to_unsigned(3187125, 23));
	--first_transmit_start_counter_44k1 <= std_logic_vector(to_unsigned(3458100, 23));
	--wd_aes_clk_timeout <= std_logic_vector(to_unsigned(38, 6));
	--wd_aes_rx_dv_timeout <= std_logic_vector(to_unsigned(11250, 15));
	--mdix_timer_1ms_reference <= std_logic_vector(to_unsigned(75000, 17));
	--aes_clk_ok_counter_reference <= std_logic_vector(to_unsigned(750000, 20));
	--mult_clk625_48k <= std_logic_vector(to_unsigned(6184753, 32));
	--mult_clk625_44k1 <= std_logic_vector(to_unsigned(5682242, 32));
end behavioral;