//Verilog instantiation template

pll _inst (.aes50_rmii_txd_data(), .aes50_rmii_txd_dout(), .aes50_rmii_txd_clkout(), 
    .aes50_rmii_txd_refclk(), .aes50_rmii_txd_reset(), .lattice_pll_CLKI(), 
    .lattice_pll_CLKOP(), .lattice_pll_CLKOS(), .lattice_pll_RST(), .lattice_pll_audio_CLKI(), 
    .lattice_pll_audio_CLKOP(), .lattice_pll_audio_CLKOS(), .lattice_pll_audio_RST(), 
    .aes50_clk_ddr_data(), .aes50_clk_ddr_dout(), .aes50_clk_ddr_clkout(), 
    .aes50_clk_ddr_refclk(), .aes50_clk_ddr_reset());