# primary clocks
#create_clock -name {clk16M} -period 62.5 [get_ports fpgaclk]
#create_clock -name {clk49_152M} -period 20.345052083 [get_ports PLL_OUT]

# generated clocks from primary clock 16 MHz
#create_generated_clock -name {clk100M_GEN} -source [get_ports fpgaclk] -master_clock clk16M -multiply_by 25 -divided_by 4 [get_nets clk_100MHz]
#create_generated_clock -name {clk50M_GEN} -source [get_ports fpgaclk] -master_clock clk16M -multiply_by 25 -divided_by 8 [get_nets clk_50MHz]

# generated clocks from primary clock 49.152 MHz
#create_generated_clock -name {clk24_576M_GEN} -source [get_ports PLL_OUT] -master_clock clk49_152M -multiply_by 1 -divided_by 2 [get_nets clk_49_152MHz]
#create_generated_clock -name {clk12_288M_GEN} -source [get_ports PLL_OUT] -master_clock clk49_152M -multiply_by 1 -divided_by 4 [get_nets clk_49_152MHz]

create_clock -period 10.00 clk_100MHz
create_clock -period 20.00 clk_50MHz
create_clock -period 20.3450520833 clk_49_152MHz
create_clock -period 40.690104166 clk_24_576MHz
create_clock -period 81.38020833 clk_12_288MHz

# timing-constraints
set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 10.00
set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 10.00
