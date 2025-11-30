create_clock -name {fpgaclk} -period 62.5 [get_ports fpgaclk]
create_clock -name {clk_24_576MHz} -period 40.69 [get_ports PLL_OUT]