-- ===========================================================================
-- OpenX32 - Thin Xilinx Spartan-3A (XC3S1400A-4FT256) Top-Level Wrapper
--
-- Pure VHDL board wrapper around openx32_core.vhd.
-- Instantiates ONLY Spartan-3A primitives (dcm2, aes50_rmii_rxd/txd,
-- oddr_clock, BUFG) and exposes G_PORTB_MODE for building all 3 switchable
-- Port B bitstreams ("AES50", "ACE", "INFERNO").
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
library UNISIM;
use UNISIM.Vcomponents.all;

entity top_spartan3a is
	generic (
		G_PORTB_MODE   : string  := "AES50"; -- "AES50", "ACE", or "INFERNO"
		G_ENABLE_SRC_A : boolean := true;
		G_ENABLE_SRC_B : boolean := true
	);
	port (
		FPGACLK                 : in  std_logic;
		PLL_OUT                 : in  std_logic;
		PLL_AUX                 : in  std_logic;
		DA_RX                   : in  std_logic;
		AD0_RX                  : in  std_logic;
		AD1_RX                  : in  std_logic;
		CARD_RX                 : in  std_logic;
		CARD_RDY                : in  std_logic;
		imx25_uart3_txd         : in  std_logic;
		imx25_uart4_txd         : in  std_logic;
		AD0_DATA1               : in  std_logic;
		AD0_DATA0               : in  std_logic;
		AD1_DATA1               : in  std_logic;
		AD1_DATA0               : in  std_logic;
		CARD_IN3                : in  std_logic;
		CARD_IN2                : in  std_logic;
		CARD_IN1                : in  std_logic;
		CARD_IN0                : in  std_logic;
		DSP_DOUT0               : in  std_logic;
		DSP_DOUT1               : in  std_logic;
		DSP_POUT0               : in  std_logic;
		DSP_POUT1               : in  std_logic;
		DSP_DOUTAUX             : in  std_logic;
		AUX_AD                  : in  std_logic;
		SPI_nCS0                : in  std_logic;
		SPI_CLK                 : in  std_logic;
		SPI_MOSI                : in  std_logic;
		aes50a_rmii_crs_dv_in   : in  std_logic;
		aes50a_rmii_rxd_0_in    : in  std_logic;
		aes50a_rmii_rxd_1_in    : in  std_logic;
		aes50a_rmii_er_in       : in  std_logic;
		aes50a_clk_a_rx_in      : in  std_logic;
		aes50a_clk_b_rx_in      : in  std_logic;
		aes50b_rmii_crs_dv_in   : in  std_logic;
		aes50b_rmii_rxd_0_in    : in  std_logic;
		aes50b_rmii_rxd_1_in    : in  std_logic;
		aes50b_rmii_er_in       : in  std_logic;
		aes50b_clk_a_rx_in      : in  std_logic;
		aes50b_clk_b_rx_in      : in  std_logic;

		PLL_nCS                 : out std_logic;
		PLL_CCLK                : out std_logic;
		PLL_CDATA               : out std_logic;
		PLL_IN                  : out std_logic;
		DA_TX                   : out std_logic;
		AD0_TX                  : out std_logic;
		AD1_TX                  : out std_logic;
		CARD_TX                 : out std_logic;
		imx25_uart3_rxd         : out std_logic;
		imx25_uart4_rxd         : out std_logic;
		AD0_MCLK                : out std_logic;
		AD0_BCLK                : out std_logic;
		AD0_FSYNC               : out std_logic;
		AD1_MCLK                : out std_logic;
		AD1_BCLK                : out std_logic;
		AD1_FSYNC               : out std_logic;
		CARD_BCLK               : out std_logic;
		CARD_FSYNC              : out std_logic;
		D_CLK                   : out std_logic;
		D_FS                    : out std_logic;
		D_CLK2                  : out std_logic;
		D_FS2                   : out std_logic;
		DA_MCLK                 : out std_logic;
		DA_BCLK                 : out std_logic;
		DA_FSYNC                : out std_logic;
		AUX_MCLK                : out std_logic;
		AUX_SCLK                : out std_logic;
		AUX_FSYNC               : out std_logic;
		AUX_CCLK                : out std_logic;
		AUX_CDATA               : out std_logic;
		AUX_nRST                : out std_logic;
		AUX_nCS                 : out std_logic;
		DA_nRESET               : out std_logic;
		AD_nRESET               : out std_logic;
		SPI_MISO                : out std_logic;
		DSP_DIN0                : out std_logic;
		DSP_DIN1                : out std_logic;
		DSP_DIN2                : out std_logic;
		DSP_DIN3                : out std_logic;
		DSP_DINAUX              : out std_logic;
		CARD_OUT0               : out std_logic;
		CARD_OUT1               : out std_logic;
		CARD_OUT2               : out std_logic;
		CARD_OUT3               : out std_logic;
		AUX_DA                  : out std_logic;
		DA_DATA1                : out std_logic;
		DA_DATA0                : out std_logic;
		P16_A_TXM               : out std_logic;
		P16_A_TXP               : out std_logic;
		P16_B_TXP               : out std_logic;
		P16_B_TXM               : out std_logic;
		aes50a_rmii_clk_out     : out std_logic;
		aes50a_rmii_tx_en_out   : out std_logic;
		aes50a_rmii_txd_0_out   : out std_logic;
		aes50a_rmii_txd_1_out   : out std_logic;
		aes50a_phy_rst_n_out    : out std_logic;
		aes50a_clk_a_tx_out     : out std_logic;
		aes50a_clk_a_tx_en_out  : out std_logic;
		aes50a_clk_a_rx_nen_out : out std_logic;
		aes50a_clk_b_tx_out     : out std_logic;
		aes50a_clk_b_tx_en_out  : out std_logic;
		aes50a_clk_b_rx_nen_out : out std_logic;
		aes50b_rmii_clk_out     : out std_logic;
		aes50b_rmii_tx_en_out   : out std_logic;
		aes50b_rmii_txd_0_out   : out std_logic;
		aes50b_rmii_txd_1_out   : out std_logic;
		aes50b_clk_a_tx_out     : out std_logic;
		aes50b_clk_a_tx_en_out  : out std_logic;
		aes50b_clk_a_rx_nen_out : out std_logic;
		aes50b_clk_b_tx_out     : out std_logic;
		aes50b_clk_b_tx_en_out  : out std_logic;
		aes50b_clk_b_rx_nen_out : out std_logic
	);
end entity top_spartan3a;

architecture rtl of top_spartan3a is

	component dcm2
		port (CLKIN_IN : in std_logic; RST_IN : in std_logic;
		      CLKFX_OUT : out std_logic; CLKFX180_OUT : out std_logic;
		      CLK0_OUT : out std_logic; LOCKED_OUT : out std_logic);
	end component;

	component oddr_clock
		port (clk_in : in std_logic; clk_in_inv : in std_logic; reset : in std_logic;
		      d0 : in std_logic; d1 : in std_logic; Q : out std_logic);
	end component;

	component aes50_rmii_rxd
		port (clk_in : in std_logic; clk_in_inv : in std_logic; reset : in std_logic;
		      rmii_crs_dv : in std_logic; rmii_rxd0 : in std_logic; rmii_rxd1 : in std_logic;
		      q : out std_logic_vector(2 downto 0));
	end component;

	component aes50_rmii_txd
		port (clk_in : in std_logic; clk_in_inv : in std_logic; reset : in std_logic;
		      data : in std_logic_vector(2 downto 0);
		      rmii_tx_en : out std_logic; rmii_txd0 : out std_logic; rmii_txd1 : out std_logic);
	end component;

	signal clk_49_152MHz  : std_logic;
	signal clk_50MHz      : std_logic;
	signal clk_50MHz_inv  : std_logic;
	signal clk_24_576MHz  : std_logic;
	signal clk_12_288MHz  : std_logic;
	signal tdm_fs         : std_logic;
	signal pripll_rst     : std_logic;
	signal rst            : std_logic;
	signal online         : std_logic;

	signal a_rxd_iob, a_txd_iob : std_logic_vector(2 downto 0);
	signal b_rxd_iob, b_txd_iob : std_logic_vector(2 downto 0);
	signal card_in_vec, card_out_vec : std_logic_vector(3 downto 0);
	signal dsp_dout_vec, dsp_din_vec : std_logic_vector(4 downto 0);

begin

	u_bufg_pll : BUFG port map (I => PLL_OUT, O => clk_49_152MHz);

	-- Single DCM: 16 MHz -> 50 MHz & 50 MHz inverted (dcm1/100 MHz no longer needed!)
	u_dcm50 : dcm2
		port map (CLKIN_IN => FPGACLK, RST_IN => pripll_rst,
		          CLKFX_OUT => clk_50MHz, CLKFX180_OUT => clk_50MHz_inv,
		          CLK0_OUT => open, LOCKED_OUT => open);

	-- Port A RMII IOB registers & ODDR 50 MHz clock
	u_a_rxd : aes50_rmii_rxd
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          rmii_crs_dv => aes50a_rmii_crs_dv_in,
		          rmii_rxd0 => aes50a_rmii_rxd_0_in, rmii_rxd1 => aes50a_rmii_rxd_1_in,
		          q => a_rxd_iob);

	u_a_txd : aes50_rmii_txd
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          data => a_txd_iob,
		          rmii_tx_en => aes50a_rmii_tx_en_out,
		          rmii_txd0 => aes50a_rmii_txd_0_out, rmii_txd1 => aes50a_rmii_txd_1_out);

	u_a_clk : oddr_clock
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          d0 => '1', d1 => '0', Q => aes50a_rmii_clk_out);

	-- Port B RMII IOB registers & ODDR 50 MHz clock
	u_b_rxd : aes50_rmii_rxd
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          rmii_crs_dv => aes50b_rmii_crs_dv_in,
		          rmii_rxd0 => aes50b_rmii_rxd_0_in, rmii_rxd1 => aes50b_rmii_rxd_1_in,
		          q => b_rxd_iob);

	u_b_txd : aes50_rmii_txd
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          data => b_txd_iob,
		          rmii_tx_en => aes50b_rmii_tx_en_out,
		          rmii_txd0 => aes50b_rmii_txd_0_out, rmii_txd1 => aes50b_rmii_txd_1_out);

	u_b_clk : oddr_clock
		port map (clk_in => clk_50MHz, clk_in_inv => clk_50MHz_inv, reset => rst,
		          d0 => '1', d1 => '0', Q => aes50b_rmii_clk_out);

	imx25_uart4_rxd <= '1';

	AD0_MCLK   <= clk_12_288MHz; AD0_BCLK   <= clk_12_288MHz; AD0_FSYNC  <= tdm_fs;
	AD1_MCLK   <= clk_12_288MHz; AD1_BCLK   <= clk_12_288MHz; AD1_FSYNC  <= tdm_fs;
	CARD_BCLK  <= clk_12_288MHz; CARD_FSYNC <= tdm_fs;
	D_CLK      <= clk_12_288MHz; D_FS       <= tdm_fs;
	D_CLK2     <= clk_12_288MHz; D_FS2      <= tdm_fs;
	DA_MCLK    <= clk_12_288MHz; DA_BCLK    <= clk_12_288MHz; DA_FSYNC   <= tdm_fs;
	AUX_MCLK   <= clk_12_288MHz; AUX_SCLK   <= clk_12_288MHz; AUX_FSYNC  <= tdm_fs;
	DA_nRESET  <= online;        AD_nRESET  <= online;

	card_in_vec  <= CARD_IN3 & CARD_IN2 & CARD_IN1 & CARD_IN0;
	CARD_OUT0    <= card_out_vec(0); CARD_OUT1 <= card_out_vec(1);
	CARD_OUT2    <= card_out_vec(2); CARD_OUT3 <= card_out_vec(3);
	dsp_dout_vec <= DSP_DOUTAUX & DSP_POUT1 & DSP_POUT0 & DSP_DOUT1 & DSP_DOUT0;
	DSP_DIN0     <= dsp_din_vec(0); DSP_DIN1 <= dsp_din_vec(1);
	DSP_DIN2     <= dsp_din_vec(2); DSP_DIN3 <= dsp_din_vec(3); DSP_DINAUX <= dsp_din_vec(4);

	u_core : entity work.openx32_core
		generic map (
			G_PORTB_MODE   => G_PORTB_MODE,
			G_BOARD_TYPE   => "SPARTAN3A",
			G_ENABLE_SRC_A => G_ENABLE_SRC_A,
			G_ENABLE_SRC_B => G_ENABLE_SRC_B
		)
		port map (
			clk_16MHz_i           => FPGACLK,
			clk_49_152MHz_i       => clk_49_152MHz,
			clk_50MHz_i           => clk_50MHz,
			pripll_rst_o          => pripll_rst,
			global_rst_o          => rst,
			online_o              => online,
			clk_24_576MHz_o       => clk_24_576MHz,
			clk_12_288MHz_o       => clk_12_288MHz,
			tdm_fs_o              => tdm_fs,
			pll_aux_i             => PLL_AUX,
			pll_in_o              => PLL_IN,
			pll_ncs_o             => PLL_nCS,
			pll_cclk_o            => PLL_CCLK,
			pll_cdata_o           => PLL_CDATA,
			imx25_uart3_txd_i     => imx25_uart3_txd,
			imx25_uart3_rxd_o     => imx25_uart3_rxd,
			da_rx_i               => DA_RX,
			ad0_rx_i              => AD0_RX,
			ad1_rx_i              => AD1_RX,
			card_rx_i             => CARD_RX,
			card_rdy_i            => CARD_RDY,
			da_tx_o               => DA_TX,
			ad0_tx_o              => AD0_TX,
			ad1_tx_o              => AD1_TX,
			card_tx_o             => CARD_TX,
			spi_ncs0_i            => SPI_nCS0,
			spi_clk_i             => SPI_CLK,
			spi_mosi_i            => SPI_MOSI,
			spi_miso_o            => SPI_MISO,
			aux_cclk_o            => AUX_CCLK,
			aux_cdata_o           => AUX_CDATA,
			aux_nrst_o            => AUX_nRST,
			aux_ad_ncs_o          => AUX_nCS,
			aux_da_ncs_o          => open,
			ad0_data0_i           => AD0_DATA0,
			ad0_data1_i           => AD0_DATA1,
			ad1_data0_i           => AD1_DATA0,
			ad1_data1_i           => AD1_DATA1,
			card_in_i             => card_in_vec,
			aux_ad_i              => AUX_AD,
			dsp_dout_i            => dsp_dout_vec,
			da_data0_o            => DA_DATA0,
			da_data1_o            => DA_DATA1,
			card_out_o            => card_out_vec,
			aux_da_o              => AUX_DA,
			dsp_din_o             => dsp_din_vec,
			p16_a_txp_o           => P16_A_TXP,
			p16_a_txm_o           => P16_A_TXM,
			p16_b_txp_o           => P16_B_TXP,
			p16_b_txm_o           => P16_B_TXM,
			aes50a_rmii_crs_dv_i  => a_rxd_iob(0),
			aes50a_rmii_rxd_i     => a_rxd_iob(2 downto 1),
			aes50a_rmii_tx_en_o   => a_txd_iob(0),
			aes50a_rmii_txd_o     => a_txd_iob(2 downto 1),
			aes50a_phy_rst_n_o    => aes50a_phy_rst_n_out,
			aes50a_clk_a_rx_i     => aes50a_clk_a_rx_in,
			aes50a_clk_a_tx_o     => aes50a_clk_a_tx_out,
			aes50a_clk_a_tx_en_o  => aes50a_clk_a_tx_en_out,
			aes50a_clk_a_rx_nen_o => aes50a_clk_a_rx_nen_out,
			aes50a_clk_b_rx_i     => aes50a_clk_b_rx_in,
			aes50a_clk_b_tx_o     => aes50a_clk_b_tx_out,
			aes50a_clk_b_tx_en_o  => aes50a_clk_b_tx_en_out,
			aes50a_clk_b_rx_nen_o => aes50a_clk_b_rx_nen_out,
			aes50b_rmii_crs_dv_i  => b_rxd_iob(0),
			aes50b_rmii_rxd_i     => b_rxd_iob(2 downto 1),
			aes50b_rmii_er_i      => aes50b_rmii_er_in,
			aes50b_rmii_tx_en_o   => b_txd_iob(0),
			aes50b_rmii_txd_o     => b_txd_iob(2 downto 1),
			aes50b_clk_a_rx_i     => aes50b_clk_a_rx_in,
			aes50b_clk_a_tx_o     => aes50b_clk_a_tx_out,
			aes50b_clk_a_tx_en_o  => aes50b_clk_a_tx_en_out,
			aes50b_clk_a_rx_nen_o => aes50b_clk_a_rx_nen_out,
			aes50b_clk_b_rx_i     => aes50b_clk_b_rx_in,
			aes50b_clk_b_tx_o     => aes50b_clk_b_tx_out,
			aes50b_clk_b_tx_en_o  => aes50b_clk_b_tx_en_out,
			aes50b_clk_b_rx_nen_o => aes50b_clk_b_rx_nen_out
		);

end architecture rtl;
