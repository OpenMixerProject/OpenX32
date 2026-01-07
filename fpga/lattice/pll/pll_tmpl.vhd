--VHDL instantiation template

component pll is
    port (aes50_clk_ddr_data: in std_logic_vector(1 downto 0);
        aes50_clk_ddr_dout: out std_logic_vector(0 downto 0);
        aes50_clk_ddr_clkout: out std_logic;
        aes50_clk_ddr_refclk: in std_logic;
        aes50_clk_ddr_reset: in std_logic;
        lattice_pll_CLKI: in std_logic;
        lattice_pll_CLKOP: out std_logic;
        lattice_pll_CLKOS: out std_logic;
        lattice_pll_RST: in std_logic;
        lattice_pll_audio_CLKI: in std_logic;
        lattice_pll_audio_CLKOP: out std_logic;
        lattice_pll_audio_CLKOS: out std_logic;
        lattice_pll_audio_RST: in std_logic
    );
    
end component pll; -- sbp_module=true 
_inst: pll port map (aes50_clk_ddr_data => __,aes50_clk_ddr_dout => __,aes50_clk_ddr_clkout => __,
          aes50_clk_ddr_refclk => __,aes50_clk_ddr_reset => __,lattice_pll_CLKI => __,
          lattice_pll_CLKOP => __,lattice_pll_CLKOS => __,lattice_pll_RST => __,
          lattice_pll_audio_CLKI => __,lattice_pll_audio_CLKOP => __,lattice_pll_audio_CLKOS => __,
          lattice_pll_audio_RST => __);
