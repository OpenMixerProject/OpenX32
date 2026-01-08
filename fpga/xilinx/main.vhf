--------------------------------------------------------------------------------
-- Copyright (c) 1995-2013 Xilinx, Inc.  All rights reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 14.7
--  \   \         Application : sch2hdl
--  /   /         Filename : main.vhf
-- /___/   /\     Timestamp : 01/08/2026 10:23:13
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
   port ( AD0_DATA0       : in    std_logic; 
          AD0_DATA1       : in    std_logic; 
          AD0_RX          : in    std_logic; 
          AD1_DATA0       : in    std_logic; 
          AD1_DATA1       : in    std_logic; 
          AD1_RX          : in    std_logic; 
          AUX_AD          : in    std_logic; 
          CARD_IN0        : in    std_logic; 
          CARD_IN1        : in    std_logic; 
          CARD_IN2        : in    std_logic; 
          CARD_IN3        : in    std_logic; 
          CARD_RX         : in    std_logic; 
          DA_RX           : in    std_logic; 
          DSP_DOUTAUX     : in    std_logic; 
          DSP_DOUT0       : in    std_logic; 
          DSP_DOUT1       : in    std_logic; 
          DSP_POUT0       : in    std_logic; 
          DSP_POUT1       : in    std_logic; 
          FPGACLK         : in    std_logic; 
          imx25_uart3_txd : in    std_logic; 
          imx25_uart4_txd : in    std_logic; 
          PLL_AUX         : in    std_logic; 
          PLL_OUT         : in    std_logic; 
          SPI_CLK         : in    std_logic; 
          SPI_MOSI        : in    std_logic; 
          SPI_nCS0        : in    std_logic; 
          AD_nRESET       : out   std_logic; 
          AD0_BCLK        : out   std_logic; 
          AD0_FSYNC       : out   std_logic; 
          AD0_MCLK        : out   std_logic; 
          AD0_TX          : out   std_logic; 
          AD1_BCLK        : out   std_logic; 
          AD1_FSYNC       : out   std_logic; 
          AD1_MCLK        : out   std_logic; 
          AD1_TX          : out   std_logic; 
          AUX_CCLK        : out   std_logic; 
          AUX_CDATA       : out   std_logic; 
          AUX_DA          : out   std_logic; 
          AUX_FSYNC       : out   std_logic; 
          AUX_MCLK        : out   std_logic; 
          AUX_nCS         : out   std_logic; 
          AUX_nRST        : out   std_logic; 
          AUX_SCLK        : out   std_logic; 
          CARD_BCLK       : out   std_logic; 
          CARD_FSYNC      : out   std_logic; 
          CARD_nRESET     : out   std_logic; 
          CARD_OUT0       : out   std_logic; 
          CARD_OUT1       : out   std_logic; 
          CARD_OUT2       : out   std_logic; 
          CARD_OUT3       : out   std_logic; 
          CARD_TX         : out   std_logic; 
          DA_BCLK         : out   std_logic; 
          DA_DATA0        : out   std_logic; 
          DA_DATA1        : out   std_logic; 
          DA_FSYNC        : out   std_logic; 
          DA_MCLK         : out   std_logic; 
          DA_nRESET       : out   std_logic; 
          DA_TX           : out   std_logic; 
          DSP_DINAUX      : out   std_logic; 
          DSP_DIN0        : out   std_logic; 
          DSP_DIN1        : out   std_logic; 
          DSP_DIN2        : out   std_logic; 
          DSP_DIN3        : out   std_logic; 
          D_CLK           : out   std_logic; 
          D_CLK2          : out   std_logic; 
          D_FS            : out   std_logic; 
          D_FS2           : out   std_logic; 
          imx25_uart3_rxd : out   std_logic; 
          imx25_uart4_rxd : out   std_logic; 
          PLL_CCLK        : out   std_logic; 
          PLL_CDATA       : out   std_logic; 
          PLL_IN          : out   std_logic; 
          PLL_nCS         : out   std_logic; 
          P16_A_TXM       : out   std_logic; 
          P16_A_TXP       : out   std_logic; 
          P16_B_TXM       : out   std_logic; 
          P16_B_TXP       : out   std_logic; 
          SPI_MISO        : out   std_logic);
end main;

architecture BEHAVIORAL of main is
   attribute BOX_TYPE         : string ;
   attribute IOSTANDARD       : string ;
   attribute IBUF_DELAY_VALUE : string ;
   signal audio_output    : std_logic_vector (479 downto 0);
   signal clk_12_288MHz   : std_logic;
   signal clk_16MHz       : std_logic;
   signal clk_24_576MHz   : std_logic;
   signal clk_49_152MHz   : std_logic;
   signal online          : std_logic;
   signal pll_locked      : std_logic;
   signal pripll_rst      : std_logic;
   signal rst             : std_logic;
   signal secpll_rst      : std_logic;
   signal start           : std_logic;
   signal tdm_fs          : std_logic;
   signal tdm_input       : std_logic_vector (19 downto 0);
   signal tdm_output      : std_logic_vector (19 downto 0);
   signal XLXN_67         : std_logic_vector (7 downto 0);
   signal XLXN_68         : std_logic_vector (7 downto 0);
   signal XLXN_69         : std_logic_vector (7 downto 0);
   signal XLXN_70         : std_logic;
   signal XLXN_222        : std_logic;
   signal XLXN_223        : std_logic_vector (7 downto 0);
   signal XLXN_224        : std_logic_vector (7 downto 0);
   signal XLXN_225        : std_logic_vector (7 downto 0);
   signal XLXN_226        : std_logic;
   signal XLXN_346        : std_logic;
   signal XLXN_1898       : std_logic;
   signal XLXN_2448       : std_logic_vector (7 downto 0);
   signal XLXN_2449       : std_logic_vector (7 downto 0);
   signal XLXN_2459       : std_logic_vector (7 downto 0);
   signal XLXN_2460       : std_logic;
   signal XLXN_2461       : std_logic_vector (7 downto 0);
   signal XLXN_2995       : std_logic_vector (479 downto 0);
   signal XLXN_2996       : std_logic_vector (23 downto 0);
   signal XLXN_2997       : std_logic_vector (7 downto 0);
   signal XLXN_2998       : std_logic_vector (23 downto 0);
   signal XLXN_3049       : std_logic_vector (23 downto 0);
   signal XLXN_3050       : std_logic_vector (23 downto 0);
   signal XLXN_3051       : std_logic_vector (23 downto 0);
   signal XLXN_3052       : std_logic_vector (23 downto 0);
   signal XLXN_3053       : std_logic_vector (23 downto 0);
   signal XLXN_3054       : std_logic_vector (23 downto 0);
   signal XLXN_3055       : std_logic_vector (23 downto 0);
   signal XLXN_3056       : std_logic_vector (23 downto 0);
   signal XLXN_3057       : std_logic_vector (23 downto 0);
   signal XLXN_3058       : std_logic_vector (23 downto 0);
   signal XLXN_3059       : std_logic_vector (23 downto 0);
   signal XLXN_3060       : std_logic_vector (23 downto 0);
   signal XLXN_3061       : std_logic_vector (23 downto 0);
   signal XLXN_3062       : std_logic_vector (23 downto 0);
   signal XLXN_3063       : std_logic_vector (23 downto 0);
   signal XLXN_3064       : std_logic_vector (23 downto 0);
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
   
   component uart_collector
      port ( clk_in   : in    std_logic; 
             rst_in   : in    std_logic; 
             uart1_in : in    std_logic; 
             uart2_in : in    std_logic; 
             uart3_in : in    std_logic; 
             uart4_in : in    std_logic; 
             uart_out : out   std_logic);
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
   
   component oddr_clock
      port ( clk_in  : in    std_logic; 
             reset   : in    std_logic; 
             clk_out : out   std_logic);
   end component;
   
   component IBUFG
      port ( I : in    std_logic; 
             O : out   std_logic);
   end component;
   attribute IOSTANDARD of IBUFG : component is "DEFAULT";
   attribute IBUF_DELAY_VALUE of IBUFG : component is "0";
   attribute BOX_TYPE of IBUFG : component is "BLACK_BOX";
   
   component BUFG
      port ( I : in    std_logic; 
             O : out   std_logic);
   end component;
   attribute BOX_TYPE of BUFG : component is "BLACK_BOX";
   
   component dcm_audioclk
      port ( CLKIN_IN        : in    std_logic; 
             RST_IN          : in    std_logic; 
             CLKDV_OUT       : out   std_logic; 
             CLKFX_OUT       : out   std_logic; 
             CLKIN_IBUFG_OUT : out   std_logic; 
             CLK0_OUT        : out   std_logic; 
             LOCKED_OUT      : out   std_logic);
   end component;
   
   component wordclock
      port ( i_clk : in    std_logic; 
             o_fs  : out   std_logic);
   end component;
   
begin
   XLXI_89 : BUF
      port map (I=>tdm_fs,
                O=>AUX_FSYNC);
   
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
   
   XLXI_165 : BUF
      port map (I=>tdm_fs,
                O=>AD0_FSYNC);
   
   XLXI_174 : BUF
      port map (I=>tdm_fs,
                O=>AD1_FSYNC);
   
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
   
   XLXI_575 : BUF
      port map (I=>online,
                O=>CARD_nRESET);
   
   XLXI_580 : audiomatrix_ram_write
      port map (bclk=>clk_12_288MHz,
                clk=>clk_24_576MHz,
                fsync=>tdm_fs,
                input_data(479 downto 0)=>XLXN_2995(479 downto 0),
                o_ram_data(23 downto 0)=>XLXN_2996(23 downto 0),
                o_ram_write_addr(7 downto 0)=>XLXN_2997(7 downto 0),
                o_ram_wr_en=>XLXN_1898);
   
   XLXI_703 : BUF
      port map (I=>tdm_fs,
                O=>D_FS);
   
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
   
   XLXI_763 : uart_collector
      port map (clk_in=>clk_16MHz,
                rst_in=>rst,
                uart1_in=>DA_RX,
                uart2_in=>AD0_RX,
                uart3_in=>AD1_RX,
                uart4_in=>CARD_RX,
                uart_out=>imx25_uart3_rxd);
   
   XLXI_764 : BUF
      port map (I=>imx25_uart4_txd,
                O=>imx25_uart4_rxd);
   
   XLXI_769 : audiomatrix_routing_ram
      port map (cfg_wr_addr(7 downto 0)=>XLXN_2459(7 downto 0),
                cfg_wr_data(7 downto 0)=>XLXN_2461(7 downto 0),
                cfg_wr_en=>XLXN_2460,
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
                o_cfg_wr_en=>XLXN_2460);
   
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
   
   XLXI_1276 : oddr_clock
      port map (clk_in=>clk_16MHz,
                reset=>pripll_rst,
                clk_out=>PLL_IN);
   
   XLXI_1277 : BUF
      port map (I=>PLL_AUX,
                O=>pll_locked);
   
   XLXI_1280 : IBUFG
      port map (I=>FPGACLK,
                O=>clk_16MHz);
   
   XLXI_1289 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AD0_MCLK);
   
   XLXI_1290 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AD0_BCLK);
   
   XLXI_1291 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AD1_MCLK);
   
   XLXI_1292 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AD1_BCLK);
   
   XLXI_1293 : BUFG
      port map (I=>clk_12_288MHz,
                O=>CARD_BCLK);
   
   XLXI_1294 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AUX_MCLK);
   
   XLXI_1295 : BUFG
      port map (I=>clk_12_288MHz,
                O=>AUX_SCLK);
   
   XLXI_1296 : BUFG
      port map (I=>clk_12_288MHz,
                O=>D_CLK);
   
   XLXI_1297 : BUFG
      port map (I=>clk_12_288MHz,
                O=>D_CLK2);
   
   XLXI_1298 : BUFG
      port map (I=>clk_12_288MHz,
                O=>DA_MCLK);
   
   XLXI_1299 : BUFG
      port map (I=>clk_12_288MHz,
                O=>DA_BCLK);
   
   XLXI_1306 : dcm_audioclk
      port map (CLKIN_IN=>PLL_OUT,
                RST_IN=>secpll_rst,
                CLKDV_OUT=>clk_12_288MHz,
                CLKFX_OUT=>clk_24_576MHz,
                CLKIN_IBUFG_OUT=>open,
                CLK0_OUT=>clk_49_152MHz,
                LOCKED_OUT=>open);
   
   XLXI_1311 : wordclock
      port map (i_clk=>clk_12_288MHz,
                o_fs=>tdm_fs);
   
end BEHAVIORAL;


