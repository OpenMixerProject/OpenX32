--------------------------------------------------------------------------------
-- Copyright (c) 1995-2013 Xilinx, Inc.  All rights reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 14.7
--  \   \         Application : sch2hdl
--  /   /         Filename : main.vhf
-- /___/   /\     Timestamp : 03/01/2026 15:51:10
-- \   \  /  \ 
--  \___\/\___\ 
--
--Command: sch2hdl -intstyle ise -family spartan3a -flat -suppress -vhdl O:/fpga/xilinx/main.vhf -w O:/fpga/xilinx/main.sch
--Design Name: main
--Device: spartan3a
--Purpose:
--    This vhdl netlist is translated from an ECS schematic. It can be 
--    synthesized and simulated, but it should not be modified. 
--

library ieee;
use ieee.std_logic_1164.ALL;
use ieee.numeric_std.ALL;
library UNISIM;
use UNISIM.Vcomponents.ALL;

entity main is
   port ( AD0_DATA0               : in    std_logic; 
          AD0_DATA1               : in    std_logic; 
          AD0_RX                  : in    std_logic; 
          AD1_DATA0               : in    std_logic; 
          AD1_DATA1               : in    std_logic; 
          AD1_RX                  : in    std_logic; 
          aes50a_clk_a_rx_in      : in    std_logic; 
          aes50a_clk_b_rx_in      : in    std_logic; 
          aes50a_rmii_crs_dv_in   : in    std_logic; 
          aes50a_rmii_rxd_0_in    : in    std_logic; 
          aes50a_rmii_rxd_1_in    : in    std_logic; 
          AUX_AD                  : in    std_logic; 
          CARD_IN0                : in    std_logic; 
          CARD_IN1                : in    std_logic; 
          CARD_IN2                : in    std_logic; 
          CARD_IN3                : in    std_logic; 
          CARD_RDY                : in    std_logic; 
          CARD_RX                 : in    std_logic; 
          DA_RX                   : in    std_logic; 
          DSP_DOUTAUX             : in    std_logic; 
          DSP_DOUT0               : in    std_logic; 
          DSP_DOUT1               : in    std_logic; 
          DSP_POUT0               : in    std_logic; 
          DSP_POUT1               : in    std_logic; 
          FPGACLK                 : in    std_logic; 
          imx25_uart3_txd         : in    std_logic; 
          imx25_uart4_txd         : in    std_logic; 
          PLL_AUX                 : in    std_logic; 
          PLL_OUT                 : in    std_logic; 
          SPI_CLK                 : in    std_logic; 
          SPI_MOSI                : in    std_logic; 
          SPI_nCS0                : in    std_logic; 
          AD_nRESET               : out   std_logic; 
          AD0_BCLK                : out   std_logic; 
          AD0_FSYNC               : out   std_logic; 
          AD0_MCLK                : out   std_logic; 
          AD0_TX                  : out   std_logic; 
          AD1_BCLK                : out   std_logic; 
          AD1_FSYNC               : out   std_logic; 
          AD1_MCLK                : out   std_logic; 
          AD1_TX                  : out   std_logic; 
          aes50a_clk_a_rx_nen_out : out   std_logic; 
          aes50a_clk_a_tx_en_out  : out   std_logic; 
          aes50a_clk_a_tx_out     : out   std_logic; 
          aes50a_clk_b_rx_nen_out : out   std_logic; 
          aes50a_clk_b_tx_en_out  : out   std_logic; 
          aes50a_clk_b_tx_out     : out   std_logic; 
          aes50a_phy_rst_n_out    : out   std_logic; 
          aes50a_rmii_clk_out     : out   std_logic; 
          aes50a_rmii_txd_0_out   : out   std_logic; 
          aes50a_rmii_txd_1_out   : out   std_logic; 
          aes50a_rmii_tx_en_out   : out   std_logic; 
          AUX_CCLK                : out   std_logic; 
          AUX_CDATA               : out   std_logic; 
          AUX_DA                  : out   std_logic; 
          AUX_FSYNC               : out   std_logic; 
          AUX_MCLK                : out   std_logic; 
          AUX_nCS                 : out   std_logic; 
          AUX_nRST                : out   std_logic; 
          AUX_SCLK                : out   std_logic; 
          CARD_BCLK               : out   std_logic; 
          CARD_FSYNC              : out   std_logic; 
          CARD_OUT0               : out   std_logic; 
          CARD_OUT1               : out   std_logic; 
          CARD_OUT2               : out   std_logic; 
          CARD_OUT3               : out   std_logic; 
          CARD_TX                 : out   std_logic; 
          DA_BCLK                 : out   std_logic; 
          DA_DATA0                : out   std_logic; 
          DA_DATA1                : out   std_logic; 
          DA_FSYNC                : out   std_logic; 
          DA_MCLK                 : out   std_logic; 
          DA_nRESET               : out   std_logic; 
          DA_TX                   : out   std_logic; 
          DSP_DINAUX              : out   std_logic; 
          DSP_DIN0                : out   std_logic; 
          DSP_DIN1                : out   std_logic; 
          DSP_DIN2                : out   std_logic; 
          DSP_DIN3                : out   std_logic; 
          D_CLK                   : out   std_logic; 
          D_CLK2                  : out   std_logic; 
          D_FS                    : out   std_logic; 
          D_FS2                   : out   std_logic; 
          imx25_uart3_rxd         : out   std_logic; 
          imx25_uart4_rxd         : out   std_logic; 
          PLL_CCLK                : out   std_logic; 
          PLL_CDATA               : out   std_logic; 
          PLL_IN                  : out   std_logic; 
          PLL_nCS                 : out   std_logic; 
          P16_A_TXM               : out   std_logic; 
          P16_A_TXP               : out   std_logic; 
          P16_B_TXM               : out   std_logic; 
          P16_B_TXP               : out   std_logic; 
          SPI_MISO                : out   std_logic);
end main;

architecture BEHAVIORAL of main is
   attribute BOX_TYPE   : string ;
   signal aes50a_rmii_rxd         : std_logic_vector (1 downto 0);
   signal aes50a_rmii_txd         : std_logic_vector (1 downto 0);
   signal aes50a_tdm_in           : std_logic_vector (6 downto 0);
   signal aes50a_tdm_out          : std_logic_vector (6 downto 0);
   signal aes50_fs_mode           : std_logic_vector (1 downto 0);
   signal audio_output            : std_logic_vector (479 downto 0);
   signal clk_12_288MHz           : std_logic;
   signal clk_16MHz               : std_logic;
   signal clk_24_576MHz           : std_logic;
   signal clk_49_152MHz           : std_logic;
   signal clk_50MHz               : std_logic;
   signal clk_50MHz_inv           : std_logic;
   signal clk_100MHz              : std_logic;
   signal configbits              : std_logic_vector (7 downto 0);
   signal online                  : std_logic;
   signal pll_locked              : std_logic;
   signal pripll_rst              : std_logic;
   signal rst                     : std_logic;
   signal secpll_rst              : std_logic;
   signal start                   : std_logic;
   signal tdm_fs                  : std_logic;
   signal tdm_input               : std_logic_vector (19 downto 0);
   signal tdm_output              : std_logic_vector (19 downto 0);
   signal XLXN_67                 : std_logic_vector (7 downto 0);
   signal XLXN_68                 : std_logic_vector (7 downto 0);
   signal XLXN_69                 : std_logic_vector (7 downto 0);
   signal XLXN_70                 : std_logic;
   signal XLXN_222                : std_logic;
   signal XLXN_223                : std_logic_vector (7 downto 0);
   signal XLXN_224                : std_logic_vector (7 downto 0);
   signal XLXN_225                : std_logic_vector (7 downto 0);
   signal XLXN_226                : std_logic;
   signal XLXN_346                : std_logic;
   signal XLXN_1898               : std_logic;
   signal XLXN_2448               : std_logic_vector (7 downto 0);
   signal XLXN_2449               : std_logic_vector (7 downto 0);
   signal XLXN_2459               : std_logic_vector (7 downto 0);
   signal XLXN_2461               : std_logic_vector (7 downto 0);
   signal XLXN_2995               : std_logic_vector (479 downto 0);
   signal XLXN_2996               : std_logic_vector (23 downto 0);
   signal XLXN_2997               : std_logic_vector (7 downto 0);
   signal XLXN_2998               : std_logic_vector (23 downto 0);
   signal XLXN_3049               : std_logic_vector (23 downto 0);
   signal XLXN_3050               : std_logic_vector (23 downto 0);
   signal XLXN_3051               : std_logic_vector (23 downto 0);
   signal XLXN_3052               : std_logic_vector (23 downto 0);
   signal XLXN_3053               : std_logic_vector (23 downto 0);
   signal XLXN_3054               : std_logic_vector (23 downto 0);
   signal XLXN_3055               : std_logic_vector (23 downto 0);
   signal XLXN_3056               : std_logic_vector (23 downto 0);
   signal XLXN_3057               : std_logic_vector (23 downto 0);
   signal XLXN_3058               : std_logic_vector (23 downto 0);
   signal XLXN_3059               : std_logic_vector (23 downto 0);
   signal XLXN_3060               : std_logic_vector (23 downto 0);
   signal XLXN_3061               : std_logic_vector (23 downto 0);
   signal XLXN_3062               : std_logic_vector (23 downto 0);
   signal XLXN_3063               : std_logic_vector (23 downto 0);
   signal XLXN_3064               : std_logic_vector (23 downto 0);
   signal XLXN_3065               : std_logic_vector (19 downto 0);
   signal XLXN_3066               : std_logic_vector (22 downto 0);
   signal XLXN_3067               : std_logic_vector (22 downto 0);
   signal XLXN_3068               : std_logic_vector (5 downto 0);
   signal XLXN_3069               : std_logic_vector (14 downto 0);
   signal XLXN_3070               : std_logic_vector (16 downto 0);
   signal XLXN_3071               : std_logic_vector (19 downto 0);
   signal XLXN_3072               : std_logic_vector (31 downto 0);
   signal XLXN_3073               : std_logic_vector (31 downto 0);
   signal XLXN_3074               : std_logic;
   signal XLXN_3086               : std_logic;
   signal XLXN_3087               : std_logic;
   signal XLXN_3088               : std_logic;
   signal XLXN_3089               : std_logic;
   signal XLXN_3115               : std_logic;
   signal XLXN_3123               : std_logic;
   signal XLXN_3287               : std_logic;
   signal XLXN_3465               : std_logic;
   signal XLXN_3479               : std_logic;
   signal XLXN_3480               : std_logic;
   signal XLXN_4443               : std_logic;
   signal XLXN_4445               : std_logic;
   signal XLXN_4526               : std_logic;
   component audioclk
      port ( fs_x_1024_i : in    std_logic; 
             fs_x_512_o  : out   std_logic; 
             fs_x_256_o  : out   std_logic; 
             fs_o        : out   std_logic);
   end component;
   
   component BUF
      port ( I : in    std_logic; 
             O : out   std_logic);
   end component;
   attribute BOX_TYPE of BUF : component is "BLACK_BOX";
   
   component spi_tx
      port ( clk       : in    std_logic; 
             i_start   : in    std_logic; 
             i_address : in    std_logic_vector (7 downto 0); 
             i_map     : in    std_logic_vector (7 downto 0); 
             i_data    : in    std_logic_vector (7 downto 0); 
             o_nCS     : out   std_logic; 
             o_cclk    : out   std_logic; 
             o_cdata   : out   std_logic; 
             o_busy    : out   std_logic);
   end component;
   
   component cs42438_config
      port ( clk       : in    std_logic; 
             i_start   : in    std_logic; 
             i_txbusy  : in    std_logic; 
             o_nRST    : out   std_logic; 
             o_start   : out   std_logic; 
             o_address : out   std_logic_vector (7 downto 0); 
             o_map     : out   std_logic_vector (7 downto 0); 
             o_data    : out   std_logic_vector (7 downto 0));
   end component;
   
   component cs2000cp_config
      port ( clk       : in    std_logic; 
             i_start   : in    std_logic; 
             i_txbusy  : in    std_logic; 
             o_start   : out   std_logic; 
             o_address : out   std_logic_vector (7 downto 0); 
             o_map     : out   std_logic_vector (7 downto 0); 
             o_data    : out   std_logic_vector (7 downto 0));
   end component;
   
   component BUFG
      port ( I : in    std_logic; 
             O : out   std_logic);
   end component;
   attribute BOX_TYPE of BUFG : component is "BLACK_BOX";
   
   component audiomatrix_ram_write
      port ( clk              : in    std_logic; 
             o_ram_wr_en      : out   std_logic; 
             o_ram_write_addr : out   std_logic_vector (7 downto 0); 
             o_ram_data       : out   std_logic_vector (23 downto 0); 
             bclk             : in    std_logic; 
             fsync            : in    std_logic; 
             input_data       : in    std_logic_vector (479 downto 0));
   end component;
   
   component audiomatrix_ram_read
      port ( clk             : in    std_logic; 
             bclk            : in    std_logic; 
             fsync           : in    std_logic; 
             ram_data        : in    std_logic_vector (23 downto 0); 
             channel_idx_out : out   std_logic_vector (7 downto 0); 
             data_out        : out   std_logic_vector (479 downto 0));
   end component;
   
   component ultranet_tx
      port ( bit_clock      : in    std_logic; 
             ch1            : in    std_logic_vector (23 downto 0); 
             ch2            : in    std_logic_vector (23 downto 0); 
             ch3            : in    std_logic_vector (23 downto 0); 
             ch4            : in    std_logic_vector (23 downto 0); 
             ch5            : in    std_logic_vector (23 downto 0); 
             ch6            : in    std_logic_vector (23 downto 0); 
             ch7            : in    std_logic_vector (23 downto 0); 
             ch8            : in    std_logic_vector (23 downto 0); 
             ultranet_out_p : out   std_logic; 
             ultranet_out_m : out   std_logic);
   end component;
   
   component reset
      port ( clk          : in    std_logic; 
             o_pripll_rst : out   std_logic; 
             o_secpll_rst : out   std_logic; 
             o_global_rst : out   std_logic; 
             o_online     : out   std_logic);
   end component;
   
   component audiomatrix_routing_ram
      port ( clk         : in    std_logic; 
             cfg_wr_en   : in    std_logic; 
             cfg_wr_addr : in    std_logic_vector (7 downto 0); 
             cfg_wr_data : in    std_logic_vector (7 downto 0); 
             read_addr   : out   std_logic_vector (7 downto 0); 
             channel_idx : in    std_logic_vector (7 downto 0));
   end component;
   
   component audiomatrix_ram
      port ( clk        : in    std_logic; 
             wr_en      : in    std_logic; 
             o_data     : out   std_logic_vector (23 downto 0); 
             write_addr : in    std_logic_vector (7 downto 0); 
             i_data     : in    std_logic_vector (23 downto 0); 
             read_addr  : in    std_logic_vector (7 downto 0));
   end component;
   
   component spi_rx_routing
      port ( clk           : in    std_logic; 
             i_spi_ncs     : in    std_logic; 
             i_spi_clk     : in    std_logic; 
             i_spi_data    : in    std_logic; 
             o_cfg_wr_en   : out   std_logic; 
             o_cfg_wr_addr : out   std_logic_vector (7 downto 0); 
             o_cfg_wr_data : out   std_logic_vector (7 downto 0));
   end component;
   
   component tdm_multichan_rx
      port ( bclk     : in    std_logic; 
             fsync    : in    std_logic; 
             tdm_in   : in    std_logic_vector (19 downto 0); 
             data_out : out   std_logic_vector (479 downto 0));
   end component;
   
   component tdm_multichan_tx
      port ( bclk    : in    std_logic; 
             fsync   : in    std_logic; 
             data_in : in    std_logic_vector (479 downto 0); 
             tdm_out : out   std_logic_vector (19 downto 0));
   end component;
   
   component tdm_demux
      port ( bclk    : in    std_logic; 
             fsync   : in    std_logic; 
             data_in : in    std_logic_vector (23 downto 0); 
             ch1_out : out   std_logic_vector (23 downto 0); 
             ch2_out : out   std_logic_vector (23 downto 0); 
             ch3_out : out   std_logic_vector (23 downto 0); 
             ch4_out : out   std_logic_vector (23 downto 0); 
             ch5_out : out   std_logic_vector (23 downto 0); 
             ch6_out : out   std_logic_vector (23 downto 0); 
             ch7_out : out   std_logic_vector (23 downto 0); 
             ch8_out : out   std_logic_vector (23 downto 0));
   end component;
   
   component aes50_top
      port ( clk50_i                             : in    std_logic; 
             clk100_i                            : in    std_logic; 
             rst_i                               : in    std_logic; 
             tdm8_i2s_mode_i                     : in    std_logic; 
             rmii_crs_dv_i                       : in    std_logic; 
             aes50_clk_a_rx_i                    : in    std_logic; 
             aes50_clk_b_rx_i                    : in    std_logic; 
             clk_1024xfs_from_pll_i              : in    std_logic; 
             pll_lock_n_i                        : in    std_logic; 
             pll_init_busy_i                     : in    std_logic; 
             wclk_readback_i                     : in    std_logic; 
             bclk_readback_i                     : in    std_logic; 
             i2s_i                               : in    std_logic; 
             fs_mode_i                           : in    std_logic_vector (1 
            downto 0); 
             sys_mode_i                          : in    std_logic_vector (1 
            downto 0); 
             rmii_rxd_i                          : in    std_logic_vector (1 
            downto 0); 
             tdm_i                               : in    std_logic_vector (6 
            downto 0); 
             debug_out_signal_pulse_len_i        : in    std_logic_vector (19 
            downto 0); 
             first_transmit_start_counter_48k_i  : in    std_logic_vector (22 
            downto 0); 
             first_transmit_start_counter_44k1_i : in    std_logic_vector (22 
            downto 0); 
             wd_aes_clk_timeout_i                : in    std_logic_vector (5 
            downto 0); 
             wd_aes_rx_dv_timeout_i              : in    std_logic_vector (14 
            downto 0); 
             mdix_timer_1ms_reference_i          : in    std_logic_vector (16 
            downto 0); 
             aes_clk_ok_counter_reference_i      : in    std_logic_vector (19 
            downto 0); 
             mult_clk625_48k_i                   : in    std_logic_vector (31 
            downto 0); 
             mult_clk625_44k1_i                  : in    std_logic_vector (31 
            downto 0); 
             rmii_tx_en_o                        : out   std_logic; 
             phy_rst_n_o                         : out   std_logic; 
             aes50_clk_a_tx_o                    : out   std_logic; 
             aes50_clk_a_tx_en_o                 : out   std_logic; 
             aes50_clk_b_tx_o                    : out   std_logic; 
             aes50_clk_b_tx_en_o                 : out   std_logic; 
             clk_to_pll_o                        : out   std_logic; 
             mclk_o                              : out   std_logic; 
             wclk_o                              : out   std_logic; 
             bclk_o                              : out   std_logic; 
             wclk_out_en_o                       : out   std_logic; 
             bclk_out_en_o                       : out   std_logic; 
             i2s_o                               : out   std_logic; 
             aes_ok_o                            : out   std_logic; 
             rmii_txd_o                          : out   std_logic_vector (1 
            downto 0); 
             pll_mult_value_o                    : out   std_logic_vector (31 
            downto 0); 
             tdm_o                               : out   std_logic_vector (6 
            downto 0); 
             dbg_o                               : out   std_logic_vector (7 
            downto 0));
   end component;
   
   component aes50_consts
      port ( debug_out_signal_pulse_len        : out   std_logic_vector (19 
            downto 0); 
             first_transmit_start_counter_48k  : out   std_logic_vector (22 
            downto 0); 
             first_transmit_start_counter_44k1 : out   std_logic_vector (22 
            downto 0); 
             wd_aes_clk_timeout                : out   std_logic_vector (5 
            downto 0); 
             wd_aes_rx_dv_timeout              : out   std_logic_vector (14 
            downto 0); 
             mdix_timer_1ms_reference          : out   std_logic_vector (16 
            downto 0); 
             aes_clk_ok_counter_reference      : out   std_logic_vector (19 
            downto 0); 
             mult_clk625_48k                   : out   std_logic_vector (31 
            downto 0); 
             mult_clk625_44k1                  : out   std_logic_vector (31 
            downto 0));
   end component;
   
   component aes50_rst
      port ( clk100_i : in    std_logic; 
             start_i  : in    std_logic; 
             rst_o    : out   std_logic);
   end component;
   
   component GND
      port ( G : out   std_logic);
   end component;
   attribute BOX_TYPE of GND : component is "BLACK_BOX";
   
   component VCC
      port ( P : out   std_logic);
   end component;
   attribute BOX_TYPE of VCC : component is "BLACK_BOX";
   
   component dcm1
      port ( CLKIN_IN   : in    std_logic; 
             RST_IN     : in    std_logic; 
             LOCKED_OUT : out   std_logic; 
             CLKFX_OUT  : out   std_logic; 
             CLK0_OUT   : out   std_logic);
   end component;
   
   component dcm2
      port ( CLKIN_IN     : in    std_logic; 
             RST_IN       : in    std_logic; 
             LOCKED_OUT   : out   std_logic; 
             CLKFX_OUT    : out   std_logic; 
             CLK0_OUT     : out   std_logic; 
             CLKFX180_OUT : out   std_logic);
   end component;
   
   component oddr_clock
      port ( clk_in     : in    std_logic; 
             clk_out    : out   std_logic; 
             reset      : in    std_logic; 
             d0         : in    std_logic; 
             d1         : in    std_logic; 
             clk_in_inv : in    std_logic);
   end component;
   
   component uart_collector
      port ( clk_in   : in    std_logic; 
             rst_in   : in    std_logic; 
             uart1_in : in    std_logic; 
             uart2_in : in    std_logic; 
             uart3_in : in    std_logic; 
             uart4_in : in    std_logic; 
             card_rdy : in    std_logic; 
             uart_out : out   std_logic);
   end component;
   
   component config_rxd
      port ( clk         : in    std_logic; 
             cfg_wr_en   : in    std_logic; 
             cfg_wr_addr : in    std_logic_vector (7 downto 0); 
             cfg_wr_data : in    std_logic_vector (7 downto 0); 
             config_bits : out   std_logic_vector (7 downto 0));
   end component;
   
   component iddr_clock
      port ( clk_in     : in    std_logic; 
             q0         : out   std_logic; 
             q1         : out   std_logic; 
             reset      : in    std_logic; 
             d          : in    std_logic; 
             clk_in_inv : in    std_logic);
   end component;
   
begin
   XLXI_53 : audioclk
      port map (fs_x_1024_i=>clk_49_152MHz,
                fs_o=>tdm_fs,
                fs_x_256_o=>clk_12_288MHz,
                fs_x_512_o=>clk_24_576MHz);
   
   XLXI_87 : BUF
      port map (I=>clk_12_288MHz,
                O=>AUX_MCLK);
   
   XLXI_88 : BUF
      port map (I=>clk_12_288MHz,
                O=>AUX_SCLK);
   
   XLXI_89 : BUF
      port map (I=>tdm_fs,
                O=>AUX_FSYNC);
   
   XLXI_96 : BUF
      port map (I=>clk_12_288MHz,
                O=>DA_MCLK);
   
   XLXI_97 : BUF
      port map (I=>clk_12_288MHz,
                O=>DA_BCLK);
   
   XLXI_98 : BUF
      port map (I=>tdm_fs,
                O=>DA_FSYNC);
   
   XLXI_109 : BUF
      port map (I=>imx25_uart3_txd,
                O=>DA_TX);
   
   XLXI_122 : spi_tx
      port map (clk=>clk_16MHz,
                i_address(7 downto 0)=>XLXN_67(7 downto 0),
                i_data(7 downto 0)=>XLXN_69(7 downto 0),
                i_map(7 downto 0)=>XLXN_68(7 downto 0),
                i_start=>XLXN_346,
                o_busy=>XLXN_70,
                o_cclk=>AUX_CCLK,
                o_cdata=>AUX_CDATA,
                o_nCS=>AUX_nCS);
   
   XLXI_123 : spi_tx
      port map (clk=>clk_16MHz,
                i_address(7 downto 0)=>XLXN_223(7 downto 0),
                i_data(7 downto 0)=>XLXN_225(7 downto 0),
                i_map(7 downto 0)=>XLXN_224(7 downto 0),
                i_start=>XLXN_222,
                o_busy=>XLXN_226,
                o_cclk=>PLL_CCLK,
                o_cdata=>PLL_CDATA,
                o_nCS=>PLL_nCS);
   
   XLXI_124 : cs42438_config
      port map (clk=>clk_16MHz,
                i_start=>start,
                i_txbusy=>XLXN_70,
                o_address(7 downto 0)=>XLXN_67(7 downto 0),
                o_data(7 downto 0)=>XLXN_69(7 downto 0),
                o_map(7 downto 0)=>XLXN_68(7 downto 0),
                o_nRST=>AUX_nRST,
                o_start=>XLXN_346);
   
   XLXI_126 : cs2000cp_config
      port map (clk=>clk_16MHz,
                i_start=>pripll_rst,
                i_txbusy=>XLXN_226,
                o_address(7 downto 0)=>XLXN_223(7 downto 0),
                o_data(7 downto 0)=>XLXN_225(7 downto 0),
                o_map(7 downto 0)=>XLXN_224(7 downto 0),
                o_start=>XLXN_222);
   
   XLXI_130 : BUF
      port map (I=>imx25_uart3_txd,
                O=>AD0_TX);
   
   XLXI_131 : BUF
      port map (I=>imx25_uart3_txd,
                O=>AD1_TX);
   
   XLXI_163 : BUF
      port map (I=>clk_12_288MHz,
                O=>AD0_MCLK);
   
   XLXI_164 : BUF
      port map (I=>clk_12_288MHz,
                O=>AD0_BCLK);
   
   XLXI_165 : BUF
      port map (I=>tdm_fs,
                O=>AD0_FSYNC);
   
   XLXI_172 : BUF
      port map (I=>clk_12_288MHz,
                O=>AD1_MCLK);
   
   XLXI_173 : BUF
      port map (I=>clk_12_288MHz,
                O=>AD1_BCLK);
   
   XLXI_174 : BUF
      port map (I=>tdm_fs,
                O=>AD1_FSYNC);
   
   XLXI_359 : BUF
      port map (I=>clk_12_288MHz,
                O=>CARD_BCLK);
   
   XLXI_360 : BUF
      port map (I=>tdm_fs,
                O=>CARD_FSYNC);
   
   XLXI_452 : BUF
      port map (I=>imx25_uart3_txd,
                O=>CARD_TX);
   
   XLXI_556 : BUF
      port map (I=>online,
                O=>DA_nRESET);
   
   XLXI_557 : BUF
      port map (I=>online,
                O=>AD_nRESET);
   
   XLXI_577 : BUFG
      port map (I=>PLL_OUT,
                O=>clk_49_152MHz);
   
   XLXI_580 : audiomatrix_ram_write
      port map (bclk=>clk_12_288MHz,
                clk=>clk_24_576MHz,
                fsync=>tdm_fs,
                input_data(479 downto 0)=>XLXN_2995(479 downto 0),
                o_ram_data(23 downto 0)=>XLXN_2996(23 downto 0),
                o_ram_write_addr(7 downto 0)=>XLXN_2997(7 downto 0),
                o_ram_wr_en=>XLXN_1898);
   
   XLXI_702 : BUF
      port map (I=>clk_12_288MHz,
                O=>D_CLK);
   
   XLXI_703 : BUF
      port map (I=>tdm_fs,
                O=>D_FS);
   
   XLXI_704 : BUF
      port map (I=>clk_24_576MHz,
                O=>D_CLK2);
   
   XLXI_705 : BUF
      port map (I=>tdm_fs,
                O=>D_FS2);
   
   XLXI_711 : audiomatrix_ram_read
      port map (bclk=>clk_12_288MHz,
                clk=>clk_24_576MHz,
                fsync=>tdm_fs,
                ram_data(23 downto 0)=>XLXN_2998(23 downto 0),
                channel_idx_out(7 downto 0)=>XLXN_2449(7 downto 0),
                data_out(479 downto 0)=>audio_output(479 downto 0));
   
   XLXI_733 : ultranet_tx
      port map (bit_clock=>clk_24_576MHz,
                ch1(23 downto 0)=>XLXN_3049(23 downto 0),
                ch2(23 downto 0)=>XLXN_3050(23 downto 0),
                ch3(23 downto 0)=>XLXN_3051(23 downto 0),
                ch4(23 downto 0)=>XLXN_3052(23 downto 0),
                ch5(23 downto 0)=>XLXN_3053(23 downto 0),
                ch6(23 downto 0)=>XLXN_3054(23 downto 0),
                ch7(23 downto 0)=>XLXN_3055(23 downto 0),
                ch8(23 downto 0)=>XLXN_3056(23 downto 0),
                ultranet_out_m=>P16_A_TXM,
                ultranet_out_p=>P16_A_TXP);
   
   XLXI_734 : ultranet_tx
      port map (bit_clock=>clk_24_576MHz,
                ch1(23 downto 0)=>XLXN_3057(23 downto 0),
                ch2(23 downto 0)=>XLXN_3058(23 downto 0),
                ch3(23 downto 0)=>XLXN_3059(23 downto 0),
                ch4(23 downto 0)=>XLXN_3060(23 downto 0),
                ch5(23 downto 0)=>XLXN_3061(23 downto 0),
                ch6(23 downto 0)=>XLXN_3062(23 downto 0),
                ch7(23 downto 0)=>XLXN_3063(23 downto 0),
                ch8(23 downto 0)=>XLXN_3064(23 downto 0),
                ultranet_out_m=>P16_B_TXM,
                ultranet_out_p=>P16_B_TXP);
   
   XLXI_740 : reset
      port map (clk=>clk_16MHz,
                o_global_rst=>rst,
                o_online=>online,
                o_pripll_rst=>pripll_rst,
                o_secpll_rst=>secpll_rst);
   
   XLXI_764 : BUF
      port map (I=>imx25_uart4_txd,
                O=>imx25_uart4_rxd);
   
   XLXI_769 : audiomatrix_routing_ram
      port map (cfg_wr_addr(7 downto 0)=>XLXN_2459(7 downto 0),
                cfg_wr_data(7 downto 0)=>XLXN_2461(7 downto 0),
                cfg_wr_en=>XLXN_3287,
                channel_idx(7 downto 0)=>XLXN_2449(7 downto 0),
                clk=>clk_24_576MHz,
                read_addr(7 downto 0)=>XLXN_2448(7 downto 0));
   
   XLXI_770 : audiomatrix_ram
      port map (clk=>clk_24_576MHz,
                i_data(23 downto 0)=>XLXN_2996(23 downto 0),
                read_addr(7 downto 0)=>XLXN_2448(7 downto 0),
                write_addr(7 downto 0)=>XLXN_2997(7 downto 0),
                wr_en=>XLXN_1898,
                o_data(23 downto 0)=>XLXN_2998(23 downto 0));
   
   XLXI_772 : BUF
      port map (I=>SPI_MOSI,
                O=>SPI_MISO);
   
   XLXI_773 : spi_rx_routing
      port map (clk=>clk_24_576MHz,
                i_spi_clk=>SPI_CLK,
                i_spi_data=>SPI_MOSI,
                i_spi_ncs=>SPI_nCS0,
                o_cfg_wr_addr(7 downto 0)=>XLXN_2459(7 downto 0),
                o_cfg_wr_data(7 downto 0)=>XLXN_2461(7 downto 0),
                o_cfg_wr_en=>XLXN_3287);
   
   XLXI_774 : BUF
      port map (I=>AUX_AD,
                O=>tdm_input(8));
   
   XLXI_784 : BUF
      port map (I=>AD0_DATA1,
                O=>tdm_input(0));
   
   XLXI_785 : BUF
      port map (I=>AD0_DATA0,
                O=>tdm_input(1));
   
   XLXI_786 : BUF
      port map (I=>AD1_DATA0,
                O=>tdm_input(3));
   
   XLXI_787 : BUF
      port map (I=>AD1_DATA1,
                O=>tdm_input(2));
   
   XLXI_838 : BUF
      port map (I=>CARD_IN3,
                O=>tdm_input(4));
   
   XLXI_839 : BUF
      port map (I=>CARD_IN1,
                O=>tdm_input(6));
   
   XLXI_840 : BUF
      port map (I=>CARD_IN0,
                O=>tdm_input(7));
   
   XLXI_841 : BUF
      port map (I=>CARD_IN2,
                O=>tdm_input(5));
   
   XLXI_876 : BUF
      port map (I=>DSP_DOUTAUX,
                O=>tdm_input(13));
   
   XLXI_877 : BUF
      port map (I=>DSP_POUT1,
                O=>tdm_input(12));
   
   XLXI_878 : BUF
      port map (I=>DSP_POUT0,
                O=>tdm_input(11));
   
   XLXI_879 : BUF
      port map (I=>DSP_DOUT1,
                O=>tdm_input(10));
   
   XLXI_880 : BUF
      port map (I=>DSP_DOUT0,
                O=>tdm_input(9));
   
   XLXI_907 : BUF
      port map (I=>tdm_output(9),
                O=>DSP_DIN0);
   
   XLXI_908 : BUF
      port map (I=>tdm_output(10),
                O=>DSP_DIN1);
   
   XLXI_909 : BUF
      port map (I=>tdm_output(11),
                O=>DSP_DIN2);
   
   XLXI_910 : BUF
      port map (I=>tdm_output(12),
                O=>DSP_DIN3);
   
   XLXI_911 : BUF
      port map (I=>tdm_output(13),
                O=>DSP_DINAUX);
   
   XLXI_922 : BUF
      port map (I=>tdm_output(4),
                O=>CARD_OUT0);
   
   XLXI_923 : BUF
      port map (I=>tdm_output(6),
                O=>CARD_OUT2);
   
   XLXI_924 : BUF
      port map (I=>tdm_output(7),
                O=>CARD_OUT3);
   
   XLXI_925 : BUF
      port map (I=>tdm_output(5),
                O=>CARD_OUT1);
   
   XLXI_943 : BUF
      port map (I=>tdm_output(0),
                O=>DA_DATA1);
   
   XLXI_944 : BUF
      port map (I=>tdm_output(1),
                O=>DA_DATA0);
   
   XLXI_975 : BUF
      port map (I=>tdm_output(8),
                O=>AUX_DA);
   
   XLXI_1074 : tdm_multichan_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                tdm_in(19 downto 0)=>tdm_input(19 downto 0),
                data_out(479 downto 0)=>XLXN_2995(479 downto 0));
   
   XLXI_1075 : tdm_multichan_tx
      port map (bclk=>clk_12_288MHz,
                data_in(479 downto 0)=>audio_output(479 downto 0),
                fsync=>tdm_fs,
                tdm_out(19 downto 0)=>tdm_output(19 downto 0));
   
   XLXI_1097 : tdm_demux
      port map (bclk=>clk_12_288MHz,
                data_in(23 downto 0)=>audio_output(71 downto 48),
                fsync=>tdm_fs,
                ch1_out(23 downto 0)=>XLXN_3049(23 downto 0),
                ch2_out(23 downto 0)=>XLXN_3050(23 downto 0),
                ch3_out(23 downto 0)=>XLXN_3051(23 downto 0),
                ch4_out(23 downto 0)=>XLXN_3052(23 downto 0),
                ch5_out(23 downto 0)=>XLXN_3053(23 downto 0),
                ch6_out(23 downto 0)=>XLXN_3054(23 downto 0),
                ch7_out(23 downto 0)=>XLXN_3055(23 downto 0),
                ch8_out(23 downto 0)=>XLXN_3056(23 downto 0));
   
   XLXI_1098 : tdm_demux
      port map (bclk=>clk_12_288MHz,
                data_in(23 downto 0)=>audio_output(95 downto 72),
                fsync=>tdm_fs,
                ch1_out(23 downto 0)=>XLXN_3057(23 downto 0),
                ch2_out(23 downto 0)=>XLXN_3058(23 downto 0),
                ch3_out(23 downto 0)=>XLXN_3059(23 downto 0),
                ch4_out(23 downto 0)=>XLXN_3060(23 downto 0),
                ch5_out(23 downto 0)=>XLXN_3061(23 downto 0),
                ch6_out(23 downto 0)=>XLXN_3062(23 downto 0),
                ch7_out(23 downto 0)=>XLXN_3063(23 downto 0),
                ch8_out(23 downto 0)=>XLXN_3064(23 downto 0));
   
   XLXI_1107 : aes50_top
      port map (aes_clk_ok_counter_reference_i(19 downto 0)=>XLXN_3071(19 
            downto 0),
                aes50_clk_a_rx_i=>aes50a_clk_a_rx_in,
                aes50_clk_b_rx_i=>aes50a_clk_b_rx_in,
                bclk_readback_i=>clk_12_288MHz,
                clk_1024xfs_from_pll_i=>clk_49_152MHz,
                clk50_i=>clk_50MHz,
                clk100_i=>clk_100MHz,
                debug_out_signal_pulse_len_i(19 downto 0)=>XLXN_3065(19 downto 
            0),
                first_transmit_start_counter_44k1_i(22 downto 0)=>XLXN_3067(22 
            downto 0),
                first_transmit_start_counter_48k_i(22 downto 0)=>XLXN_3066(22 
            downto 0),
                fs_mode_i(1 downto 0)=>aes50_fs_mode(1 downto 0),
                i2s_i=>XLXN_3089,
                mdix_timer_1ms_reference_i(16 downto 0)=>XLXN_3070(16 downto 0),
                mult_clk625_44k1_i(31 downto 0)=>XLXN_3073(31 downto 0),
                mult_clk625_48k_i(31 downto 0)=>XLXN_3072(31 downto 0),
                pll_init_busy_i=>XLXN_3088,
                pll_lock_n_i=>XLXN_3087,
                rmii_crs_dv_i=>XLXN_3465,
                rmii_rxd_i(1 downto 0)=>aes50a_rmii_rxd(1 downto 0),
                rst_i=>XLXN_3074,
                sys_mode_i(1 downto 0)=>configbits(2 downto 1),
                tdm_i(6 downto 0)=>aes50a_tdm_in(6 downto 0),
                tdm8_i2s_mode_i=>XLXN_3086,
                wclk_readback_i=>tdm_fs,
                wd_aes_clk_timeout_i(5 downto 0)=>XLXN_3068(5 downto 0),
                wd_aes_rx_dv_timeout_i(14 downto 0)=>XLXN_3069(14 downto 0),
                aes_ok_o=>open,
                aes50_clk_a_tx_en_o=>aes50a_clk_a_tx_en_out,
                aes50_clk_a_tx_o=>aes50a_clk_a_tx_out,
                aes50_clk_b_tx_en_o=>aes50a_clk_b_tx_en_out,
                aes50_clk_b_tx_o=>aes50a_clk_b_tx_out,
                bclk_o=>open,
                bclk_out_en_o=>open,
                clk_to_pll_o=>open,
                dbg_o=>open,
                i2s_o=>open,
                mclk_o=>open,
                phy_rst_n_o=>aes50a_phy_rst_n_out,
                pll_mult_value_o=>open,
                rmii_txd_o(1 downto 0)=>aes50a_rmii_txd(1 downto 0),
                rmii_tx_en_o=>XLXN_4526,
                tdm_o(6 downto 0)=>aes50a_tdm_out(6 downto 0),
                wclk_o=>open,
                wclk_out_en_o=>open);
   
   XLXI_1108 : aes50_consts
      port map (aes_clk_ok_counter_reference(19 downto 0)=>XLXN_3071(19 downto 
            0),
                debug_out_signal_pulse_len(19 downto 0)=>XLXN_3065(19 downto 0),
                first_transmit_start_counter_44k1(22 downto 0)=>XLXN_3067(22 
            downto 0),
                first_transmit_start_counter_48k(22 downto 0)=>XLXN_3066(22 
            downto 0),
                mdix_timer_1ms_reference(16 downto 0)=>XLXN_3070(16 downto 0),
                mult_clk625_44k1(31 downto 0)=>XLXN_3073(31 downto 0),
                mult_clk625_48k(31 downto 0)=>XLXN_3072(31 downto 0),
                wd_aes_clk_timeout(5 downto 0)=>XLXN_3068(5 downto 0),
                wd_aes_rx_dv_timeout(14 downto 0)=>XLXN_3069(14 downto 0));
   
   XLXI_1109 : aes50_rst
      port map (clk100_i=>clk_100MHz,
                start_i=>configbits(0),
                rst_o=>XLXN_3074);
   
   XLXI_1110 : GND
      port map (G=>XLXN_3086);
   
   XLXI_1111 : GND
      port map (G=>XLXN_3088);
   
   XLXI_1112 : GND
      port map (G=>XLXN_3087);
   
   XLXI_1113 : GND
      port map (G=>XLXN_3089);
   
   XLXI_1114 : BUF
      port map (I=>tdm_output(15),
                O=>aes50a_tdm_in(1));
   
   XLXI_1115 : BUF
      port map (I=>tdm_output(16),
                O=>aes50a_tdm_in(2));
   
   XLXI_1116 : BUF
      port map (I=>tdm_output(17),
                O=>aes50a_tdm_in(3));
   
   XLXI_1117 : BUF
      port map (I=>tdm_output(18),
                O=>aes50a_tdm_in(4));
   
   XLXI_1118 : BUF
      port map (I=>tdm_output(19),
                O=>aes50a_tdm_in(5));
   
   XLXI_1119 : BUF
      port map (I=>XLXN_3115,
                O=>aes50a_tdm_in(6));
   
   XLXI_1120 : BUF
      port map (I=>tdm_output(14),
                O=>aes50a_tdm_in(0));
   
   XLXI_1133 : GND
      port map (G=>XLXN_3115);
   
   XLXI_1134 : BUF
      port map (I=>aes50a_tdm_out(0),
                O=>tdm_input(14));
   
   XLXI_1135 : BUF
      port map (I=>aes50a_tdm_out(1),
                O=>tdm_input(15));
   
   XLXI_1136 : BUF
      port map (I=>aes50a_tdm_out(3),
                O=>tdm_input(17));
   
   XLXI_1137 : BUF
      port map (I=>aes50a_tdm_out(4),
                O=>tdm_input(18));
   
   XLXI_1138 : BUF
      port map (I=>aes50a_tdm_out(5),
                O=>tdm_input(19));
   
   XLXI_1139 : BUF
      port map (I=>aes50a_tdm_out(6),
                O=>XLXN_3123);
   
   XLXI_1140 : BUF
      port map (I=>aes50a_tdm_out(2),
                O=>tdm_input(16));
   
   XLXI_1141 : GND
      port map (G=>aes50a_clk_a_rx_nen_out);
   
   XLXI_1142 : GND
      port map (G=>aes50a_clk_b_rx_nen_out);
   
   XLXI_1143 : BUF
      port map (I=>aes50a_rmii_txd(0),
                O=>XLXN_4443);
   
   XLXI_1144 : BUF
      port map (I=>aes50a_rmii_txd(1),
                O=>XLXN_4445);
   
   XLXI_1148 : GND
      port map (G=>aes50_fs_mode(1));
   
   XLXI_1149 : VCC
      port map (P=>aes50_fs_mode(0));
   
   XLXI_1157 : dcm1
      port map (CLKIN_IN=>clk_16MHz,
                RST_IN=>pripll_rst,
                CLKFX_OUT=>clk_100MHz,
                CLK0_OUT=>open,
                LOCKED_OUT=>open);
   
   XLXI_1158 : dcm2
      port map (CLKIN_IN=>clk_16MHz,
                RST_IN=>pripll_rst,
                CLKFX_OUT=>clk_50MHz,
                CLKFX180_OUT=>clk_50MHz_inv,
                CLK0_OUT=>open,
                LOCKED_OUT=>open);
   
   XLXI_1261 : oddr_clock
      port map (clk_in=>clk_50MHz,
                clk_in_inv=>clk_50MHz_inv,
                d0=>XLXN_3479,
                d1=>XLXN_3480,
                reset=>rst,
                clk_out=>aes50a_rmii_clk_out);
   
   XLXI_1268 : uart_collector
      port map (card_rdy=>CARD_RDY,
                clk_in=>clk_16MHz,
                rst_in=>rst,
                uart1_in=>DA_RX,
                uart2_in=>AD0_RX,
                uart3_in=>AD1_RX,
                uart4_in=>CARD_RX,
                uart_out=>imx25_uart3_rxd);
   
   XLXI_1273 : BUF
      port map (I=>PLL_AUX,
                O=>pll_locked);
   
   XLXI_1276 : BUFG
      port map (I=>FPGACLK,
                O=>clk_16MHz);
   
   XLXI_1277 : BUFG
      port map (I=>FPGACLK,
                O=>PLL_IN);
   
   XLXI_1354 : config_rxd
      port map (cfg_wr_addr(7 downto 0)=>XLXN_2459(7 downto 0),
                cfg_wr_data(7 downto 0)=>XLXN_2461(7 downto 0),
                cfg_wr_en=>XLXN_3287,
                clk=>clk_24_576MHz,
                config_bits(7 downto 0)=>configbits(7 downto 0));
   
   XLXI_1388 : GND
      port map (G=>XLXN_3480);
   
   XLXI_1389 : VCC
      port map (P=>XLXN_3479);
   
   XLXI_1390 : oddr_clock
      port map (clk_in=>clk_50MHz,
                clk_in_inv=>clk_50MHz_inv,
                d0=>XLXN_4526,
                d1=>XLXN_4526,
                reset=>rst,
                clk_out=>aes50a_rmii_tx_en_out);
   
   XLXI_1391 : oddr_clock
      port map (clk_in=>clk_50MHz,
                clk_in_inv=>clk_50MHz_inv,
                d0=>XLXN_4443,
                d1=>XLXN_4443,
                reset=>rst,
                clk_out=>aes50a_rmii_txd_0_out);
   
   XLXI_1392 : oddr_clock
      port map (clk_in=>clk_50MHz,
                clk_in_inv=>clk_50MHz_inv,
                d0=>XLXN_4445,
                d1=>XLXN_4445,
                reset=>rst,
                clk_out=>aes50a_rmii_txd_1_out);
   
   XLXI_1440 : iddr_clock
      port map (clk_in=>clk_50MHz_inv,
                clk_in_inv=>clk_50MHz,
                d=>aes50a_rmii_crs_dv_in,
                reset=>rst,
                q0=>XLXN_3465,
                q1=>open);
   
   XLXI_1441 : iddr_clock
      port map (clk_in=>clk_50MHz_inv,
                clk_in_inv=>clk_50MHz,
                d=>aes50a_rmii_rxd_0_in,
                reset=>rst,
                q0=>aes50a_rmii_rxd(0),
                q1=>open);
   
   XLXI_1442 : iddr_clock
      port map (clk_in=>clk_50MHz_inv,
                clk_in_inv=>clk_50MHz,
                d=>aes50a_rmii_rxd_1_in,
                reset=>rst,
                q0=>aes50a_rmii_rxd(1),
                q1=>open);
   
end BEHAVIORAL;


