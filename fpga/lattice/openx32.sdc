# clock from external PLL (49.152 MHz)
create_clock -period 20.345 main_lattice|PLL_OUT

# clock for AES50-logic (75 MHz, 80MHz, 90MHz or 100MHz)
#create_clock -period 13.33 clk_100MHz
#create_clock -period 12.5 clk_100MHz
#create_clock -period 11.0 clk_100MHz
create_clock -period 10.0 clk_100MHz

# clock for EthernetPHY (50 MHz)
create_clock -period 20.00 clk_50MHz

# maximum delay between 50MHz and 100MHz clock
#set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 13.33
#set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 13.33
#set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 12.5
#set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 12.5
#set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 11.0
#set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 11.0
set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 10.0
set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 10.0

# unused clocks
#create_clock -period 10.00 lattice_pll|CLKOS_inferred_clock
#create_clock -period 20.00 lattice_pll|CLKOS2_inferred_clock
#create_clock -period 40.00 lattice_pll_audio|CLKOP_inferred_clock
#create_clock -period 80.00 lattice_pll_audio|CLKOS_inferred_clock

#set_max_delay -from lattice_pll|CLKOS_inferred_clock -to lattice_pll|CLKOS2_inferred_clock -datapath_only 10.0
#set_max_delay -from lattice_pll|CLKOS2_inferred_clock -to lattice_pll|CLKOS_inferred_clock -datapath_only 10.0
