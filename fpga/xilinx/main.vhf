--------------------------------------------------------------------------------
-- Copyright (c) 1995-2013 Xilinx, Inc.  All rights reserved.
--------------------------------------------------------------------------------
--   ____  ____ 
--  /   /\/   / 
-- /___/  \  /    Vendor: Xilinx 
-- \   \   \/     Version : 14.7
--  \   \         Application : sch2hdl
--  /   /         Filename : main.vhf
-- /___/   /\     Timestamp : 12/14/2025 23:44:04
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
   attribute BOX_TYPE   : string ;
   signal audiosync       : std_logic;
   signal audio_input     : std_logic_vector (2687 downto 0);
   signal audio_output    : std_logic_vector (2687 downto 0);
   signal clk_12_288MHz   : std_logic;
   signal clk_16MHz       : std_logic;
   signal clk_24_576MHz   : std_logic;
   signal nRST            : std_logic;
   signal rst             : std_logic;
   signal start           : std_logic;
   signal tdm_fs          : std_logic;
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
   signal XLXN_1973       : std_logic_vector (6 downto 0);
   signal XLXN_1974       : std_logic_vector (23 downto 0);
   signal XLXN_2357       : std_logic;
   signal XLXN_2374       : std_logic_vector (23 downto 0);
   signal XLXN_2448       : std_logic_vector (6 downto 0);
   signal XLXN_2449       : std_logic_vector (6 downto 0);
   signal XLXN_2459       : std_logic_vector (6 downto 0);
   signal XLXN_2460       : std_logic;
   signal XLXN_2461       : std_logic_vector (6 downto 0);
   component tdm_8ch_rx
      port ( bclk     : in    std_logic; 
             fsync    : in    std_logic; 
             sdata    : in    std_logic; 
             sync_out : out   std_logic; 
             ch1_out  : out   std_logic_vector (23 downto 0); 
             ch2_out  : out   std_logic_vector (23 downto 0); 
             ch3_out  : out   std_logic_vector (23 downto 0); 
             ch4_out  : out   std_logic_vector (23 downto 0); 
             ch5_out  : out   std_logic_vector (23 downto 0); 
             ch6_out  : out   std_logic_vector (23 downto 0); 
             ch7_out  : out   std_logic_vector (23 downto 0); 
             ch8_out  : out   std_logic_vector (23 downto 0));
   end component;
   
   component tdm_8ch_tx
      port ( bclk   : in    std_logic; 
             fsync  : in    std_logic; 
             ch1_in : in    std_logic_vector (23 downto 0); 
             ch2_in : in    std_logic_vector (23 downto 0); 
             ch3_in : in    std_logic_vector (23 downto 0); 
             ch4_in : in    std_logic_vector (23 downto 0); 
             ch5_in : in    std_logic_vector (23 downto 0); 
             ch6_in : in    std_logic_vector (23 downto 0); 
             ch7_in : in    std_logic_vector (23 downto 0); 
             ch8_in : in    std_logic_vector (23 downto 0); 
             sdata  : out   std_logic);
   end component;
   
   component audioclk
      port ( i_clk : in    std_logic; 
             o_clk : out   std_logic; 
             o_fs  : out   std_logic);
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
             sync_in          : in    std_logic; 
             input_data       : in    std_logic_vector (2687 downto 0); 
             o_ram_wr_en      : out   std_logic; 
             o_write_done     : out   std_logic; 
             o_ram_write_addr : out   std_logic_vector (6 downto 0); 
             o_ram_data       : out   std_logic_vector (23 downto 0));
   end component;
   
   component audiomatrix_ram_read
      port ( clk                : in    std_logic; 
             sync_in            : in    std_logic; 
             output_channel_idx : out   std_logic_vector (6 downto 0); 
             output_data        : out   std_logic_vector (2687 downto 0); 
             i_ram_data         : in    std_logic_vector (23 downto 0));
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
      port ( clk         : in    std_logic; 
             o_reset     : out   std_logic; 
             o_reset_inv : out   std_logic; 
             o_startup   : out   std_logic);
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
   
   component spi_rx_routing
      port ( clk           : in    std_logic; 
             i_spi_ncs     : in    std_logic; 
             i_spi_clk     : in    std_logic; 
             i_spi_data    : in    std_logic; 
             o_cfg_wr_en   : out   std_logic; 
             o_cfg_wr_addr : out   std_logic_vector (6 downto 0); 
             o_cfg_wr_data : out   std_logic_vector (6 downto 0));
   end component;
   
   component audiomatrix_routing_ram
      port ( clk                : in    std_logic; 
             cfg_wr_en          : in    std_logic; 
             cfg_wr_addr        : in    std_logic_vector (6 downto 0); 
             cfg_wr_data        : in    std_logic_vector (6 downto 0); 
             output_channel_idx : in    std_logic_vector (6 downto 0); 
             read_addr          : out   std_logic_vector (6 downto 0));
   end component;
   
   component audiomatrix_ram
      port ( clk        : in    std_logic; 
             wr_en      : in    std_logic; 
             write_addr : in    std_logic_vector (6 downto 0); 
             i_data     : in    std_logic_vector (23 downto 0); 
             read_addr  : in    std_logic_vector (6 downto 0); 
             o_data     : out   std_logic_vector (23 downto 0));
   end component;
   
begin
   SPI_MISO <= '0';
   XLXI_49 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>AUX_AD,
                ch1_out(23 downto 0)=>audio_input(1559 downto 1536),
                ch2_out(23 downto 0)=>audio_input(1583 downto 1560),
                ch3_out(23 downto 0)=>audio_input(1607 downto 1584),
                ch4_out(23 downto 0)=>audio_input(1631 downto 1608),
                ch5_out(23 downto 0)=>audio_input(1655 downto 1632),
                ch6_out(23 downto 0)=>audio_input(1679 downto 1656),
                ch7_out(23 downto 0)=>audio_input(1703 downto 1680),
                ch8_out(23 downto 0)=>audio_input(1727 downto 1704),
                sync_out=>audiosync);
   
   XLXI_50 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(1559 downto 1536),
                ch2_in(23 downto 0)=>audio_output(1583 downto 1560),
                ch3_in(23 downto 0)=>audio_output(1607 downto 1584),
                ch4_in(23 downto 0)=>audio_output(1631 downto 1608),
                ch5_in(23 downto 0)=>audio_output(1655 downto 1632),
                ch6_in(23 downto 0)=>audio_output(1679 downto 1656),
                ch7_in(23 downto 0)=>audio_output(1703 downto 1680),
                ch8_in(23 downto 0)=>audio_output(1727 downto 1704),
                fsync=>tdm_fs,
                sdata=>AUX_DA);
   
   XLXI_53 : audioclk
      port map (i_clk=>clk_24_576MHz,
                o_clk=>clk_12_288MHz,
                o_fs=>tdm_fs);
   
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
   
   XLXI_99 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(23 downto 0),
                ch2_in(23 downto 0)=>audio_output(47 downto 24),
                ch3_in(23 downto 0)=>audio_output(71 downto 48),
                ch4_in(23 downto 0)=>audio_output(95 downto 72),
                ch5_in(23 downto 0)=>audio_output(119 downto 96),
                ch6_in(23 downto 0)=>audio_output(143 downto 120),
                ch7_in(23 downto 0)=>audio_output(167 downto 144),
                ch8_in(23 downto 0)=>audio_output(191 downto 168),
                fsync=>tdm_fs,
                sdata=>DA_DATA1);
   
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
                i_start=>rst,
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
   
   XLXI_146 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>AD0_DATA1,
                ch1_out(23 downto 0)=>audio_input(23 downto 0),
                ch2_out(23 downto 0)=>audio_input(47 downto 24),
                ch3_out(23 downto 0)=>audio_input(71 downto 48),
                ch4_out(23 downto 0)=>audio_input(95 downto 72),
                ch5_out(23 downto 0)=>audio_input(119 downto 96),
                ch6_out(23 downto 0)=>audio_input(143 downto 120),
                ch7_out(23 downto 0)=>audio_input(167 downto 144),
                ch8_out(23 downto 0)=>audio_input(191 downto 168),
                sync_out=>open);
   
   XLXI_147 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>AD1_DATA1,
                ch1_out(23 downto 0)=>audio_input(215 downto 192),
                ch2_out(23 downto 0)=>audio_input(239 downto 216),
                ch3_out(23 downto 0)=>audio_input(263 downto 240),
                ch4_out(23 downto 0)=>audio_input(287 downto 264),
                ch5_out(23 downto 0)=>audio_input(311 downto 288),
                ch6_out(23 downto 0)=>audio_input(335 downto 312),
                ch7_out(23 downto 0)=>audio_input(359 downto 336),
                ch8_out(23 downto 0)=>audio_input(383 downto 360),
                sync_out=>open);
   
   XLXI_150 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>AD0_DATA0,
                ch1_out(23 downto 0)=>audio_input(407 downto 384),
                ch2_out(23 downto 0)=>audio_input(431 downto 408),
                ch3_out(23 downto 0)=>audio_input(455 downto 432),
                ch4_out(23 downto 0)=>audio_input(479 downto 456),
                ch5_out(23 downto 0)=>audio_input(503 downto 480),
                ch6_out(23 downto 0)=>audio_input(527 downto 504),
                ch7_out(23 downto 0)=>audio_input(551 downto 528),
                ch8_out(23 downto 0)=>audio_input(575 downto 552),
                sync_out=>open);
   
   XLXI_151 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>AD1_DATA0,
                ch1_out(23 downto 0)=>audio_input(599 downto 576),
                ch2_out(23 downto 0)=>audio_input(623 downto 600),
                ch3_out(23 downto 0)=>audio_input(647 downto 624),
                ch4_out(23 downto 0)=>audio_input(671 downto 648),
                ch5_out(23 downto 0)=>audio_input(695 downto 672),
                ch6_out(23 downto 0)=>audio_input(719 downto 696),
                ch7_out(23 downto 0)=>audio_input(743 downto 720),
                ch8_out(23 downto 0)=>audio_input(767 downto 744),
                sync_out=>open);
   
   XLXI_152 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(215 downto 192),
                ch2_in(23 downto 0)=>audio_output(239 downto 216),
                ch3_in(23 downto 0)=>audio_output(263 downto 240),
                ch4_in(23 downto 0)=>audio_output(287 downto 264),
                ch5_in(23 downto 0)=>audio_output(311 downto 288),
                ch6_in(23 downto 0)=>audio_output(335 downto 312),
                ch7_in(23 downto 0)=>audio_output(359 downto 336),
                ch8_in(23 downto 0)=>audio_output(383 downto 360),
                fsync=>tdm_fs,
                sdata=>DA_DATA0);
   
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
   
   XLXI_358 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>CARD_IN3,
                ch1_out(23 downto 0)=>audio_input(791 downto 768),
                ch2_out(23 downto 0)=>audio_input(815 downto 792),
                ch3_out(23 downto 0)=>audio_input(839 downto 816),
                ch4_out(23 downto 0)=>audio_input(863 downto 840),
                ch5_out(23 downto 0)=>audio_input(887 downto 864),
                ch6_out(23 downto 0)=>audio_input(911 downto 888),
                ch7_out(23 downto 0)=>audio_input(935 downto 912),
                ch8_out(23 downto 0)=>audio_input(959 downto 936),
                sync_out=>open);
   
   XLXI_359 : BUF
      port map (I=>clk_12_288MHz,
                O=>CARD_BCLK);
   
   XLXI_360 : BUF
      port map (I=>tdm_fs,
                O=>CARD_FSYNC);
   
   XLXI_363 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>CARD_IN2,
                ch1_out(23 downto 0)=>audio_input(983 downto 960),
                ch2_out(23 downto 0)=>audio_input(1007 downto 984),
                ch3_out(23 downto 0)=>audio_input(1031 downto 1008),
                ch4_out(23 downto 0)=>audio_input(1055 downto 1032),
                ch5_out(23 downto 0)=>audio_input(1079 downto 1056),
                ch6_out(23 downto 0)=>audio_input(1103 downto 1080),
                ch7_out(23 downto 0)=>audio_input(1127 downto 1104),
                ch8_out(23 downto 0)=>audio_input(1151 downto 1128),
                sync_out=>open);
   
   XLXI_364 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>CARD_IN1,
                ch1_out(23 downto 0)=>audio_input(1175 downto 1152),
                ch2_out(23 downto 0)=>audio_input(1199 downto 1176),
                ch3_out(23 downto 0)=>audio_input(1223 downto 1200),
                ch4_out(23 downto 0)=>audio_input(1247 downto 1224),
                ch5_out(23 downto 0)=>audio_input(1271 downto 1248),
                ch6_out(23 downto 0)=>audio_input(1295 downto 1272),
                ch7_out(23 downto 0)=>audio_input(1319 downto 1296),
                ch8_out(23 downto 0)=>audio_input(1343 downto 1320),
                sync_out=>open);
   
   XLXI_368 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>CARD_IN0,
                ch1_out(23 downto 0)=>audio_input(1367 downto 1344),
                ch2_out(23 downto 0)=>audio_input(1391 downto 1368),
                ch3_out(23 downto 0)=>audio_input(1415 downto 1392),
                ch4_out(23 downto 0)=>audio_input(1439 downto 1416),
                ch5_out(23 downto 0)=>audio_input(1463 downto 1440),
                ch6_out(23 downto 0)=>audio_input(1487 downto 1464),
                ch7_out(23 downto 0)=>audio_input(1511 downto 1488),
                ch8_out(23 downto 0)=>audio_input(1535 downto 1512),
                sync_out=>open);
   
   XLXI_405 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(983 downto 960),
                ch2_in(23 downto 0)=>audio_output(1007 downto 984),
                ch3_in(23 downto 0)=>audio_output(1031 downto 1008),
                ch4_in(23 downto 0)=>audio_output(1055 downto 1032),
                ch5_in(23 downto 0)=>audio_output(1079 downto 1056),
                ch6_in(23 downto 0)=>audio_output(1103 downto 1080),
                ch7_in(23 downto 0)=>audio_output(1127 downto 1104),
                ch8_in(23 downto 0)=>audio_output(1151 downto 1128),
                fsync=>tdm_fs,
                sdata=>CARD_OUT1);
   
   XLXI_410 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(1175 downto 1152),
                ch2_in(23 downto 0)=>audio_output(1199 downto 1176),
                ch3_in(23 downto 0)=>audio_output(1223 downto 1200),
                ch4_in(23 downto 0)=>audio_output(1247 downto 1224),
                ch5_in(23 downto 0)=>audio_output(1271 downto 1248),
                ch6_in(23 downto 0)=>audio_output(1295 downto 1272),
                ch7_in(23 downto 0)=>audio_output(1319 downto 1296),
                ch8_in(23 downto 0)=>audio_output(1343 downto 1320),
                fsync=>tdm_fs,
                sdata=>CARD_OUT2);
   
   XLXI_411 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(1367 downto 1344),
                ch2_in(23 downto 0)=>audio_output(1391 downto 1368),
                ch3_in(23 downto 0)=>audio_output(1415 downto 1392),
                ch4_in(23 downto 0)=>audio_output(1439 downto 1416),
                ch5_in(23 downto 0)=>audio_output(1463 downto 1440),
                ch6_in(23 downto 0)=>audio_output(1487 downto 1464),
                ch7_in(23 downto 0)=>audio_output(1511 downto 1488),
                ch8_in(23 downto 0)=>audio_output(1535 downto 1512),
                fsync=>tdm_fs,
                sdata=>CARD_OUT3);
   
   XLXI_415 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(791 downto 768),
                ch2_in(23 downto 0)=>audio_output(815 downto 792),
                ch3_in(23 downto 0)=>audio_output(839 downto 816),
                ch4_in(23 downto 0)=>audio_output(863 downto 840),
                ch5_in(23 downto 0)=>audio_output(887 downto 864),
                ch6_in(23 downto 0)=>audio_output(911 downto 888),
                ch7_in(23 downto 0)=>audio_output(935 downto 912),
                ch8_in(23 downto 0)=>audio_output(959 downto 936),
                fsync=>tdm_fs,
                sdata=>CARD_OUT0);
   
   XLXI_452 : BUF
      port map (I=>imx25_uart3_txd,
                O=>CARD_TX);
   
   XLXI_556 : BUF
      port map (I=>nRST,
                O=>DA_nRESET);
   
   XLXI_557 : BUF
      port map (I=>nRST,
                O=>AD_nRESET);
   
   XLXI_575 : BUF
      port map (I=>nRST,
                O=>CARD_nRESET);
   
   XLXI_577 : BUFG
      port map (I=>PLL_OUT,
                O=>clk_24_576MHz);
   
   XLXI_580 : audiomatrix_ram_write
      port map (clk=>clk_24_576MHz,
                input_data(2687 downto 0)=>audio_input(2687 downto 0),
                sync_in=>audiosync,
                o_ram_data(23 downto 0)=>XLXN_1974(23 downto 0),
                o_ram_write_addr(6 downto 0)=>XLXN_1973(6 downto 0),
                o_ram_wr_en=>XLXN_1898,
                o_write_done=>XLXN_2357);
   
   XLXI_643 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(1751 downto 1728),
                ch2_in(23 downto 0)=>audio_output(1775 downto 1752),
                ch3_in(23 downto 0)=>audio_output(1799 downto 1776),
                ch4_in(23 downto 0)=>audio_output(1823 downto 1800),
                ch5_in(23 downto 0)=>audio_output(1847 downto 1824),
                ch6_in(23 downto 0)=>audio_output(1871 downto 1848),
                ch7_in(23 downto 0)=>audio_output(1895 downto 1872),
                ch8_in(23 downto 0)=>audio_output(1919 downto 1896),
                fsync=>tdm_fs,
                sdata=>DSP_DIN0);
   
   XLXI_645 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(1943 downto 1920),
                ch2_in(23 downto 0)=>audio_output(1967 downto 1944),
                ch3_in(23 downto 0)=>audio_output(1991 downto 1968),
                ch4_in(23 downto 0)=>audio_output(2015 downto 1992),
                ch5_in(23 downto 0)=>audio_output(2039 downto 2016),
                ch6_in(23 downto 0)=>audio_output(2063 downto 2040),
                ch7_in(23 downto 0)=>audio_output(2087 downto 2064),
                ch8_in(23 downto 0)=>audio_output(2111 downto 2088),
                fsync=>tdm_fs,
                sdata=>DSP_DIN1);
   
   XLXI_646 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(2135 downto 2112),
                ch2_in(23 downto 0)=>audio_output(2159 downto 2136),
                ch3_in(23 downto 0)=>audio_output(2183 downto 2160),
                ch4_in(23 downto 0)=>audio_output(2207 downto 2184),
                ch5_in(23 downto 0)=>audio_output(2231 downto 2208),
                ch6_in(23 downto 0)=>audio_output(2255 downto 2232),
                ch7_in(23 downto 0)=>audio_output(2279 downto 2256),
                ch8_in(23 downto 0)=>audio_output(2303 downto 2280),
                fsync=>tdm_fs,
                sdata=>DSP_DIN2);
   
   XLXI_647 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(2327 downto 2304),
                ch2_in(23 downto 0)=>audio_output(2351 downto 2328),
                ch3_in(23 downto 0)=>audio_output(2375 downto 2352),
                ch4_in(23 downto 0)=>audio_output(2399 downto 2376),
                ch5_in(23 downto 0)=>audio_output(2423 downto 2400),
                ch6_in(23 downto 0)=>audio_output(2447 downto 2424),
                ch7_in(23 downto 0)=>audio_output(2471 downto 2448),
                ch8_in(23 downto 0)=>audio_output(2495 downto 2472),
                fsync=>tdm_fs,
                sdata=>DSP_DIN3);
   
   XLXI_665 : tdm_8ch_tx
      port map (bclk=>clk_12_288MHz,
                ch1_in(23 downto 0)=>audio_output(2519 downto 2496),
                ch2_in(23 downto 0)=>audio_output(2543 downto 2520),
                ch3_in(23 downto 0)=>audio_output(2567 downto 2544),
                ch4_in(23 downto 0)=>audio_output(2591 downto 2568),
                ch5_in(23 downto 0)=>audio_output(2615 downto 2592),
                ch6_in(23 downto 0)=>audio_output(2639 downto 2616),
                ch7_in(23 downto 0)=>audio_output(2663 downto 2640),
                ch8_in(23 downto 0)=>audio_output(2687 downto 2664),
                fsync=>tdm_fs,
                sdata=>DSP_DINAUX);
   
   XLXI_666 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>DSP_DOUT0,
                ch1_out(23 downto 0)=>audio_input(1751 downto 1728),
                ch2_out(23 downto 0)=>audio_input(1775 downto 1752),
                ch3_out(23 downto 0)=>audio_input(1799 downto 1776),
                ch4_out(23 downto 0)=>audio_input(1823 downto 1800),
                ch5_out(23 downto 0)=>audio_input(1847 downto 1824),
                ch6_out(23 downto 0)=>audio_input(1871 downto 1848),
                ch7_out(23 downto 0)=>audio_input(1895 downto 1872),
                ch8_out(23 downto 0)=>audio_input(1919 downto 1896),
                sync_out=>open);
   
   XLXI_669 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>DSP_DOUT1,
                ch1_out(23 downto 0)=>audio_input(1943 downto 1920),
                ch2_out(23 downto 0)=>audio_input(1967 downto 1944),
                ch3_out(23 downto 0)=>audio_input(1991 downto 1968),
                ch4_out(23 downto 0)=>audio_input(2015 downto 1992),
                ch5_out(23 downto 0)=>audio_input(2039 downto 2016),
                ch6_out(23 downto 0)=>audio_input(2063 downto 2040),
                ch7_out(23 downto 0)=>audio_input(2087 downto 2064),
                ch8_out(23 downto 0)=>audio_input(2111 downto 2088),
                sync_out=>open);
   
   XLXI_671 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>DSP_POUT0,
                ch1_out(23 downto 0)=>audio_input(2135 downto 2112),
                ch2_out(23 downto 0)=>audio_input(2159 downto 2136),
                ch3_out(23 downto 0)=>audio_input(2183 downto 2160),
                ch4_out(23 downto 0)=>audio_input(2207 downto 2184),
                ch5_out(23 downto 0)=>audio_input(2231 downto 2208),
                ch6_out(23 downto 0)=>audio_input(2255 downto 2232),
                ch7_out(23 downto 0)=>audio_input(2279 downto 2256),
                ch8_out(23 downto 0)=>audio_input(2303 downto 2280),
                sync_out=>open);
   
   XLXI_672 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>DSP_POUT1,
                ch1_out(23 downto 0)=>audio_input(2327 downto 2304),
                ch2_out(23 downto 0)=>audio_input(2351 downto 2328),
                ch3_out(23 downto 0)=>audio_input(2375 downto 2352),
                ch4_out(23 downto 0)=>audio_input(2399 downto 2376),
                ch5_out(23 downto 0)=>audio_input(2423 downto 2400),
                ch6_out(23 downto 0)=>audio_input(2447 downto 2424),
                ch7_out(23 downto 0)=>audio_input(2471 downto 2448),
                ch8_out(23 downto 0)=>audio_input(2495 downto 2472),
                sync_out=>open);
   
   XLXI_673 : tdm_8ch_rx
      port map (bclk=>clk_12_288MHz,
                fsync=>tdm_fs,
                sdata=>DSP_DOUTAUX,
                ch1_out(23 downto 0)=>audio_input(2519 downto 2496),
                ch2_out(23 downto 0)=>audio_input(2543 downto 2520),
                ch3_out(23 downto 0)=>audio_input(2567 downto 2544),
                ch4_out(23 downto 0)=>audio_input(2591 downto 2568),
                ch5_out(23 downto 0)=>audio_input(2615 downto 2592),
                ch6_out(23 downto 0)=>audio_input(2639 downto 2616),
                ch7_out(23 downto 0)=>audio_input(2663 downto 2640),
                ch8_out(23 downto 0)=>audio_input(2687 downto 2664),
                sync_out=>open);
   
   XLXI_702 : BUF
      port map (I=>clk_12_288MHz,
                O=>D_CLK);
   
   XLXI_703 : BUF
      port map (I=>tdm_fs,
                O=>D_FS);
   
   XLXI_704 : BUF
      port map (I=>clk_12_288MHz,
                O=>D_CLK2);
   
   XLXI_705 : BUF
      port map (I=>tdm_fs,
                O=>D_FS2);
   
   XLXI_711 : audiomatrix_ram_read
      port map (clk=>clk_24_576MHz,
                i_ram_data(23 downto 0)=>XLXN_2374(23 downto 0),
                sync_in=>XLXN_2357,
                output_channel_idx(6 downto 0)=>XLXN_2449(6 downto 0),
                output_data(2687 downto 0)=>audio_output(2687 downto 0));
   
   XLXI_733 : ultranet_tx
      port map (bit_clock=>clk_24_576MHz,
                ch1(23 downto 0)=>audio_output(407 downto 384),
                ch2(23 downto 0)=>audio_output(431 downto 408),
                ch3(23 downto 0)=>audio_output(455 downto 432),
                ch4(23 downto 0)=>audio_output(479 downto 456),
                ch5(23 downto 0)=>audio_output(503 downto 480),
                ch6(23 downto 0)=>audio_output(527 downto 504),
                ch7(23 downto 0)=>audio_output(551 downto 528),
                ch8(23 downto 0)=>audio_output(575 downto 552),
                ultranet_out_m=>P16_A_TXM,
                ultranet_out_p=>P16_A_TXP);
   
   XLXI_734 : ultranet_tx
      port map (bit_clock=>clk_24_576MHz,
                ch1(23 downto 0)=>audio_output(599 downto 576),
                ch2(23 downto 0)=>audio_output(623 downto 600),
                ch3(23 downto 0)=>audio_output(647 downto 624),
                ch4(23 downto 0)=>audio_output(671 downto 648),
                ch5(23 downto 0)=>audio_output(695 downto 672),
                ch6(23 downto 0)=>audio_output(719 downto 696),
                ch7(23 downto 0)=>audio_output(743 downto 720),
                ch8(23 downto 0)=>audio_output(767 downto 744),
                ultranet_out_m=>P16_B_TXM,
                ultranet_out_p=>P16_B_TXP);
   
   XLXI_740 : reset
      port map (clk=>clk_16MHz,
                o_reset=>rst,
                o_reset_inv=>nRST,
                o_startup=>start);
   
   XLXI_745 : BUF
      port map (I=>FPGACLK,
                O=>clk_16MHz);
   
   XLXI_746 : BUF
      port map (I=>FPGACLK,
                O=>PLL_IN);
   
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
   
   XLXI_768 : spi_rx_routing
      port map (clk=>clk_24_576MHz,
                i_spi_clk=>SPI_CLK,
                i_spi_data=>SPI_MOSI,
                i_spi_ncs=>SPI_nCS0,
                o_cfg_wr_addr(6 downto 0)=>XLXN_2459(6 downto 0),
                o_cfg_wr_data(6 downto 0)=>XLXN_2461(6 downto 0),
                o_cfg_wr_en=>XLXN_2460);
   
   XLXI_769 : audiomatrix_routing_ram
      port map (cfg_wr_addr(6 downto 0)=>XLXN_2459(6 downto 0),
                cfg_wr_data(6 downto 0)=>XLXN_2461(6 downto 0),
                cfg_wr_en=>XLXN_2460,
                clk=>clk_24_576MHz,
                output_channel_idx(6 downto 0)=>XLXN_2449(6 downto 0),
                read_addr(6 downto 0)=>XLXN_2448(6 downto 0));
   
   XLXI_770 : audiomatrix_ram
      port map (clk=>clk_24_576MHz,
                i_data(23 downto 0)=>XLXN_1974(23 downto 0),
                read_addr(6 downto 0)=>XLXN_2448(6 downto 0),
                write_addr(6 downto 0)=>XLXN_1973(6 downto 0),
                wr_en=>XLXN_1898,
                o_data(23 downto 0)=>XLXN_2374(23 downto 0));
   
end BEHAVIORAL;


