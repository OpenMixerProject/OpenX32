library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity debug_switch is
	port (
		card_bclk_i		: in std_logic;
		card_fsclk_i	: in std_logic;
		card_out0_i		: in std_logic;
		card_out1_i		: in std_logic;
		card_out2_i		: in std_logic;
		card_out3_i		: in std_logic;

		debug0_i			: in std_logic;
		debug1_i			: in std_logic;
		debug2_i			: in std_logic;
		debug3_i			: in std_logic;
		debug4_i			: in std_logic;
		debug5_i			: in std_logic;

		debug_signals	: in std_logic;

		card_bclk_o		: out std_logic;
		card_fsclk_o	: out std_logic;
		card_out0_o		: out std_logic;
		card_out1_o		: out std_logic;
		card_out2_o		: out std_logic;
		card_out3_o		: out std_logic
	); 		
end entity;

architecture behavioral of debug_switch is
begin
	card_bclk_o  <= card_bclk_i  when (debug_signals = '0') else (debug0_i);
	card_fsclk_o <= card_fsclk_i when (debug_signals = '0') else (debug1_i);
	card_out0_o  <= card_out0_i  when (debug_signals = '0') else (debug2_i);
	card_out1_o  <= card_out1_i  when (debug_signals = '0') else (debug3_i);
	card_out2_o  <= card_out2_i  when (debug_signals = '0') else (debug4_i);
	card_out3_o  <= card_out3_i  when (debug_signals = '0') else (debug5_i);
end behavioral;