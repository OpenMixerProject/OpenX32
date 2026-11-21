# Caution: this SDC-file is interpreted by Synplify Pro only
# for the Place and Route performed by Lattice Diamond we have to define more timing-settings in the LPF-file

# define the clocks from external hardware
create_clock -name {clk_16MHz} -period 62.5 [get_ports {fpgaclk}]
create_clock -name {clk_49_152MHz} -period 20.3450520834 [get_ports {PLL_OUT}]

# define inferred clocks from PLLs
create_clock -name {clk_50MHz} -period 20.0 [get_pins {b2v_inst_pll_phy.PLLInst_0.CLKOS}]

# BCLK (12.288 MHz) clocks AES50v2 TDM/I2S SerDes directly
create_generated_clock -name {clk_12_288MHz} -source [get_ports {PLL_OUT}] -divide_by 4 [get_nets {clk_12_288MHz}]

# define setup-and-hold-time for the 50MHz clock
set_output_delay -clock [get_clocks clk_50MHz] -max 12.0 [get_ports {aes50a_rmii_clk_out aes50a_rmii_tx_en_out aes50a_rmii_txd_0_out aes50a_rmii_txd_1_out}]
set_output_delay -clock [get_clocks clk_50MHz] -min 8.0 [get_ports {aes50a_rmii_clk_out aes50a_rmii_tx_en_out aes50a_rmii_txd_0_out aes50a_rmii_txd_1_out}]

# define groups
set_clock_groups -asynchronous \
    -group {clk_50MHz} \
    -group {clk_16MHz} \
    -group {clk_49_152MHz clk_12_288MHz}
