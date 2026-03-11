# Caution: this SDC-file is interpreted by Synplify Pro only
# for the Place and Route performed by Lattice Diamond we have to define more timing-settings in the LPF-file

# define the clocks from external hardware
create_clock -name {clk_16MHz} -period 62.5 [get_ports {fpgaclk}]
create_clock -name {clk_49_152MHz} -period 20.3450520834 [get_ports {PLL_OUT}]

# define inferred clocks from PLLs
create_clock -name {clk_100MHz} -period 10.0 [get_pins {b2v_inst74.PLLInst_0.CLKOP}]
create_clock -name {clk_50MHz} -period 20.0 [get_pins {b2v_inst74.PLLInst_0.CLKOS2}]
create_clock -name {clk_24_576MHz} -period 40.690104167 [get_pins {b2v_inst28.PLLInst_0.CLKOP}]
create_clock -name {clk_12_288MHz} -period 81.38020834 [get_pins {b2v_inst28.PLLInst_0.CLKOS}]

# if logic is used to generate sub-clocks, use the following line
#create_generated_clock -name {clk_100MHz} -source {get_ports {fpgaclk}} -multiply_by 25 -divide_by 4 [get_pins {b2v_inst74.PLLInst_0.CLKOP}]
#create_generated_clock -name {clk_50MHz} -source {get_ports {fpgaclk}} -multiply_by 25 -divide_by 8 [get_pins {b2v_inst74.PLLInst_0.CLKOS}]
#create_generated_clock -name {clk_24_576MHz} -source {get_ports {PLL_OUT}} -divide_by 2 [get_pins {b2v_inst28.PLLInst_0.CLKOP}]
#create_generated_clock -name {clk_12_288MHz} -source {get_ports {PLL_OUT}} -divide_by 4 [get_pins {b2v_inst28.PLLInst_0.CLKOS}]

# define maximum delay between 50MHz and 100MHz clocks
set_max_delay -from clk_50MHz -to clk_100MHz -datapath_only 10.0
set_max_delay -from clk_100MHz -to clk_50MHz -datapath_only 10.0

# define setup-and-hold-time for the 50MHz clock
set_output_delay -clock [get_clocks clk_50MHz] -max 12.0 [get_ports {aes50a_rmii_clk_out aes50a_rmii_tx_en_out aes50a_rmii_txd_0_out aes50a_rmii_txd_1_out}]
set_output_delay -clock [get_clocks clk_50MHz] -min 8.0 [get_ports {aes50a_rmii_clk_out aes50a_rmii_tx_en_out aes50a_rmii_txd_0_out aes50a_rmii_txd_1_out}]

# define groups
#set_clock_groups -asynchronous -group {clk_100MHz clk_50MHz} -group {clk_audio_op clk_audio_os} -group {clk_16MHz}
set_clock_groups -asynchronous \
    -group {clk_100MHz} \
    -group {clk_50MHz} \
    -group {clk_24_576MHz} \
    -group {clk_12_288MHz} \
    -group {clk_16MHz} \
	-group {clk_49_152MHz}
