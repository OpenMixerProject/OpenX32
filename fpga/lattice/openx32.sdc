create_clock -period 10.00 lattice_pll|CLKOS_inferred_clock
create_clock -period 20.00 lattice_pll|CLKOS2_inferred_clock
create_clock -period 20.00 main_lattice|PLL_OUT
create_clock -period 40.00 lattice_pll_audio|CLKOP_inferred_clock
create_clock -period 80.00 lattice_pll_audio|CLKOS_inferred_clock

set_max_delay -from lattice_pll|CLKOS_inferred_clock -to lattice_pll|CLKOS2_inferred_clock -datapath_only 10.0
set_max_delay -from lattice_pll|CLKOS2_inferred_clock -to lattice_pll|CLKOS_inferred_clock -datapath_only 10.0
