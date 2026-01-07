/* synthesis translate_off*/
`define SBP_SIMULATION
/* synthesis translate_on*/
`ifndef SBP_SIMULATION
`define SBP_SYNTHESIS
`endif

//
// Verific Verilog Description of module pll
//
module pll (aes50_clk_ddr_data, aes50_clk_ddr_dout, aes50_clk_ddr_clkout, 
            aes50_clk_ddr_refclk, aes50_clk_ddr_reset, lattice_pll_CLKI, 
            lattice_pll_CLKOP, lattice_pll_CLKOS, lattice_pll_RST, lattice_pll_audio_CLKI, 
            lattice_pll_audio_CLKOP, lattice_pll_audio_CLKOS, lattice_pll_audio_RST) /* synthesis sbp_module=true */ ;
    input [1:0]aes50_clk_ddr_data;
    output [0:0]aes50_clk_ddr_dout;
    output aes50_clk_ddr_clkout;
    input aes50_clk_ddr_refclk;
    input aes50_clk_ddr_reset;
    input lattice_pll_CLKI;
    output lattice_pll_CLKOP;
    output lattice_pll_CLKOS;
    input lattice_pll_RST;
    input lattice_pll_audio_CLKI;
    output lattice_pll_audio_CLKOP;
    output lattice_pll_audio_CLKOS;
    input lattice_pll_audio_RST;
    
    
    aes50_clk_ddr aes50_clk_ddr_inst (.data({aes50_clk_ddr_data}), .dout({aes50_clk_ddr_dout}), 
            .clkout(aes50_clk_ddr_clkout), .refclk(aes50_clk_ddr_refclk), 
            .reset(aes50_clk_ddr_reset));
    lattice_pll lattice_pll_inst (.CLKI(lattice_pll_CLKI), .CLKOP(lattice_pll_CLKOP), 
            .CLKOS(lattice_pll_CLKOS), .RST(lattice_pll_RST));
    lattice_pll_audio lattice_pll_audio_inst (.CLKI(lattice_pll_audio_CLKI), 
            .CLKOP(lattice_pll_audio_CLKOP), .CLKOS(lattice_pll_audio_CLKOS), 
            .RST(lattice_pll_audio_RST));
    
endmodule

