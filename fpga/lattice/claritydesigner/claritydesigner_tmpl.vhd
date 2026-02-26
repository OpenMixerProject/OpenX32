--VHDL instantiation template

component claritydesigner is
    port (aes50_clk_ddr_data: in std_logic_vector(1 downto 0);
        aes50_clk_ddr_dout: out std_logic_vector(0 downto 0);
        aes50_rmii_rxd_datain: in std_logic_vector(2 downto 0);
        aes50_rmii_rxd_q: out std_logic_vector(2 downto 0);
        aes50_rmii_txd_data: in std_logic_vector(2 downto 0);
        aes50_rmii_txd_dout: out std_logic_vector(2 downto 0);
        aes50_clk_ddr_clkout: out std_logic;
        aes50_clk_ddr_refclk: in std_logic;
        aes50_clk_ddr_reset: in std_logic;
        aes50_rmii_rxd_clkin: in std_logic;
        aes50_rmii_rxd_reset: in std_logic;
        aes50_rmii_rxd_sclk: out std_logic;
        aes50_rmii_txd_clkout: out std_logic;
        aes50_rmii_txd_refclk: in std_logic;
        aes50_rmii_txd_reset: in std_logic;
        lattice_pll_CLKI: in std_logic;
        lattice_pll_CLKOP: out std_logic;
        lattice_pll_CLKOS: out std_logic;
        lattice_pll_RST: in std_logic;
        lattice_pll_audio_CLKI: in std_logic;
        lattice_pll_audio_CLKOP: out std_logic;
        lattice_pll_audio_CLKOS: out std_logic;
        lattice_pll_audio_RST: in std_logic;
        lattice_pll_phy_CLKI: in std_logic;
        lattice_pll_phy_CLKOP: out std_logic;
        lattice_pll_phy_CLKOS: out std_logic;
        lattice_pll_phy_RST: in std_logic
    );
    
end component claritydesigner; -- sbp_module=true 
_inst: claritydesigner port map (lattice_pll_phy_CLKI => __,lattice_pll_phy_CLKOP => __,
            lattice_pll_phy_CLKOS => __,lattice_pll_phy_RST => __,lattice_pll_audio_CLKI => __,
            lattice_pll_audio_CLKOP => __,lattice_pll_audio_CLKOS => __,lattice_pll_audio_RST => __,
            aes50_clk_ddr_data => __,aes50_clk_ddr_dout => __,aes50_clk_ddr_clkout => __,
            aes50_clk_ddr_refclk => __,aes50_clk_ddr_reset => __,aes50_rmii_rxd_datain => __,
            aes50_rmii_rxd_q => __,aes50_rmii_rxd_clkin => __,aes50_rmii_rxd_reset => __,
            aes50_rmii_rxd_sclk => __,aes50_rmii_txd_data => __,aes50_rmii_txd_dout => __,
            aes50_rmii_txd_clkout => __,aes50_rmii_txd_refclk => __,aes50_rmii_txd_reset => __,
            lattice_pll_CLKI => __,lattice_pll_CLKOP => __,lattice_pll_CLKOS => __,
            lattice_pll_RST => __);
