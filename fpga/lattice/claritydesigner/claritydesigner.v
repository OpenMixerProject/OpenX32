/* synthesis translate_off*/
`define SBP_SIMULATION
/* synthesis translate_on*/
`ifndef SBP_SIMULATION
`define SBP_SYNTHESIS
`endif

//
// Verific Verilog Description of module claritydesigner
//
module claritydesigner (aes50_clk_ddr_data, aes50_clk_ddr_dout, aes50_rmii_rxd_datain, 
            aes50_rmii_rxd_q, aes50_rmii_txd_data, aes50_rmii_txd_dout, 
            aes50_clk_ddr_clkout, aes50_clk_ddr_refclk, aes50_clk_ddr_reset, 
            aes50_rmii_rxd_clkin, aes50_rmii_rxd_reset, aes50_rmii_rxd_sclk, 
            aes50_rmii_txd_clkout, aes50_rmii_txd_refclk, aes50_rmii_txd_reset, 
            lattice_pll_CLKI, lattice_pll_CLKOP, lattice_pll_CLKOS, lattice_pll_CLKOS2, 
            lattice_pll_RST, lattice_pll_audio_CLKI, lattice_pll_audio_CLKOP, 
            lattice_pll_audio_CLKOS, lattice_pll_audio_RST) /* synthesis sbp_module=true */ ;
    input [1:0]aes50_clk_ddr_data;
    output [0:0]aes50_clk_ddr_dout;
    input [2:0]aes50_rmii_rxd_datain;
    output [2:0]aes50_rmii_rxd_q;
    input [2:0]aes50_rmii_txd_data;
    output [2:0]aes50_rmii_txd_dout;
    output aes50_clk_ddr_clkout;
    input aes50_clk_ddr_refclk;
    input aes50_clk_ddr_reset;
    input aes50_rmii_rxd_clkin;
    input aes50_rmii_rxd_reset;
    output aes50_rmii_rxd_sclk;
    output aes50_rmii_txd_clkout;
    input aes50_rmii_txd_refclk;
    input aes50_rmii_txd_reset;
    input lattice_pll_CLKI;
    output lattice_pll_CLKOP;
    output lattice_pll_CLKOS;
    output lattice_pll_CLKOS2;
    input lattice_pll_RST;
    input lattice_pll_audio_CLKI;
    output lattice_pll_audio_CLKOP;
    output lattice_pll_audio_CLKOS;
    input lattice_pll_audio_RST;
    
    
    aes50_clk_ddr aes50_clk_ddr_inst (.data({aes50_clk_ddr_data}), .dout({aes50_clk_ddr_dout}), 
            .clkout(aes50_clk_ddr_clkout), .refclk(aes50_clk_ddr_refclk), 
            .reset(aes50_clk_ddr_reset));
    aes50_rmii_rxd aes50_rmii_rxd_inst (.datain({aes50_rmii_rxd_datain}), 
            .q({aes50_rmii_rxd_q}), .clkin(aes50_rmii_rxd_clkin), .reset(aes50_rmii_rxd_reset), 
            .sclk(aes50_rmii_rxd_sclk));
    aes50_rmii_txd aes50_rmii_txd_inst (.data({aes50_rmii_txd_data}), .dout({aes50_rmii_txd_dout}), 
            .clkout(aes50_rmii_txd_clkout), .refclk(aes50_rmii_txd_refclk), 
            .reset(aes50_rmii_txd_reset));
    lattice_pll lattice_pll_inst (.CLKI(lattice_pll_CLKI), .CLKOP(lattice_pll_CLKOP), 
            .CLKOS(lattice_pll_CLKOS), .CLKOS2(lattice_pll_CLKOS2), .RST(lattice_pll_RST));
    lattice_pll_audio lattice_pll_audio_inst (.CLKI(lattice_pll_audio_CLKI), 
            .CLKOP(lattice_pll_audio_CLKOP), .CLKOS(lattice_pll_audio_CLKOS), 
            .RST(lattice_pll_audio_RST));
    
endmodule

