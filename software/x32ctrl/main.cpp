/*
							   =#%@@@@@###%@@@@%-
						   =*###+               :#@*
						+****.                      :%-
					  #++++             ############  :%-          @@@@@@@@@@@@
					.+===             ###======++==*##  *+       @@@*#%%#****#@@@
				   -=-+               #+======+=.*===*#         @@#**@.*@******%@
				  -=-+                ##======*  #====+#.      @@****@  @******@@
				  +:-                  ##+====#  #***==+#+   =@@**@@@@  @****%@@
				 =*=*                   -#*===#  ## #===+## @@@***@ @@  @@#*@@@
	  @@@@@@       ..                     ##+*#- ## #***==#= @@@@@@ @@   +@@@   @@@@@@@  @@@@@
	@@@    @@@                             ##= + ## ## #+==#- @@ @@ @@ = @@@    @@  @@  @@   @@
	@@      .@@#@@@@@@@  @@@ @@@ @@@@@@@@   .# # ## .= #-#++#= @ @  @@ * @*       @@*        @@
	@@       @@ @@    @@ @@@@@@@  @@   @@      # =#  = + *::=#   @  @+ *           -@@@   @@@=
	@@@    @@@  @@:   @@ @@       @@   @@      #  : .- : *::-#   @  +  #             @@ @@    @@
	  @@@@@@    @@@@@@@   @@@@@@  @@   @@@  =# # ## :+ #-#++#+ @ @  @@.* @@     @@@@@@  @@@@@@@@
				@@                         ##+ * ## ## #+==#+ @@@@@ @@ = @@@
				@@                        ##+= = ## #***==#+ @@***@ @@   #@@@
				   :                    :#*==+#: ## #===+## @@@***@ @@  @@#*@@#
				  .%+                  ##+====#  #***==+#=   +@@**@@@@  @****%@@
					%.                ##======*  #====*#  .*-  @@****@  @******@@
					 %=               #*======+: *===*#   +-=+  @@#**@ -@******@@
					  -@-             +##+=====+++=*##  ==-=-    @@@#%@@#****%@@@
						*@*             =###########  -===*        @@@@@@@@@@@@
						   @@%.                   .::=++*
							 .#@@%%*-.    .:=+**##***+.
								  .-+%%%%%%#***=-.

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

// includes for timer
#include <time.h>
#include <signal.h>

#include "ctrl.h"

X32Ctrl* ctrl;
State* state;
CLI::App* app;
timer_t timerid;
struct sigevent sev;
struct itimerspec trigger;

void timer100msCallbackLvgl(_lv_timer_t* lv_timer) { 
	ctrl->Tick100ms();
}

void timer100msCallbackLinux(int timer) {
	ctrl->Tick100ms();
}

void timer10msCallbackLvgl(_lv_timer_t* lv_timer) {
	ui_tick(); ctrl->Tick10ms();
}
void timer10msCallbackLinux(int timer) {
	ctrl->Tick10ms();
}

// initialize 100ms timer (only for Non-GUI systems)
void init100msTimer(void) {
	// Set up the signal handler
	struct sigaction sa;
	sa.sa_handler = timer100msCallbackLinux;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
		perror("sigaction");
		//return 1;
	}

	// Set up the sigevent structure for the timer
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &timerid;

	// Create the timer
	if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
		perror("timer_create");
		//return 1;
	}
	
	// Set the timer to trigger every 1 second (1,000,000,000 nanoseconds)
	trigger.it_value.tv_sec = 0;
	trigger.it_value.tv_nsec = 50000000; // 50ms = 50000us = 50000000ns
	trigger.it_interval.tv_sec = 0;
	trigger.it_interval.tv_nsec = 50000000;

	// Arm the timer
	if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
		perror("timer_settime");
		//return 1;
	}
}

void guiInit(void) {
	lv_init();

	driver_backends_register();
	char dev[] = "FBDEV";
	driver_backends_init_backend(dev);

	lv_timer_create(timer10msCallbackLvgl, 10, NULL);
	lv_timer_create(timer100msCallbackLvgl, 100, NULL);

	// initialize GUI created by EEZ
	ui_init();

	ctrl->ShowPage(state->activePage);
	

	// setup Gate-graph
	lv_chart_set_type(objects.current_channel_gate, LV_CHART_TYPE_LINE);
	lv_obj_set_style_size(objects.current_channel_gate, 0, 0, LV_PART_INDICATOR);
	lv_obj_set_style_line_width(objects.current_channel_gate, 5, LV_PART_ITEMS);
	ctrl->chartSeriesGate = lv_chart_add_series(objects.current_channel_gate, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
	ctrl->chartSeriesGateAudio = lv_chart_add_series(objects.current_channel_gate, lv_palette_main(LV_PALETTE_BLUE_GREY), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_div_line_count(objects.current_channel_gate, 5, 7);
	lv_chart_set_range(objects.current_channel_gate, LV_CHART_AXIS_PRIMARY_Y, -6000, 0);
	lv_chart_set_point_count(objects.current_channel_gate, 200); // with incoming audio every 10ms we see 2 seconds of audio-history
	lv_chart_set_series_color(objects.current_channel_gate, ctrl->chartSeriesGate, lv_color_hex(0xef7900));
	//chart-shadow: 0x7e4000

	// setup Dynamics-graph
	lv_chart_set_type(objects.current_channel_comp, LV_CHART_TYPE_LINE);
	lv_obj_set_style_size(objects.current_channel_comp, 0, 0, LV_PART_INDICATOR);
	lv_obj_set_style_line_width(objects.current_channel_comp, 5, LV_PART_ITEMS);
	ctrl->chartSeriesCompressor = lv_chart_add_series(objects.current_channel_comp, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
	ctrl->chartSeriesCompressorAudio = lv_chart_add_series(objects.current_channel_comp, lv_palette_main(LV_PALETTE_BLUE_GREY), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_div_line_count(objects.current_channel_comp, 5, 7);
	lv_chart_set_range(objects.current_channel_comp, LV_CHART_AXIS_PRIMARY_Y, -6000, 0);
	lv_chart_set_point_count(objects.current_channel_comp, 200); // with incoming audio every 10ms we see 2 seconds of audio-history
	lv_chart_set_series_color(objects.current_channel_comp, ctrl->chartSeriesCompressor, lv_color_hex(0xef7900));
	//chart-shadow: 0x7e4000

	// setup EQ-graph
	lv_chart_set_type(objects.current_channel_eq, LV_CHART_TYPE_LINE);
	lv_obj_set_style_size(objects.current_channel_eq, 0, 0, LV_PART_INDICATOR);
	lv_obj_set_style_line_width(objects.current_channel_eq, 5, LV_PART_ITEMS);
	ctrl->chartSeriesEQ = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_div_line_count(objects.current_channel_eq, 5, 7);
	lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_PRIMARY_Y, -15000, 15000);
	lv_chart_set_point_count(objects.current_channel_eq, 200);
	lv_chart_set_series_color(objects.current_channel_eq, ctrl->chartSeriesEQ, lv_color_hex(0xef7900));
	//chart-shadow: 0x7e4000

	// start endless loop
	driver_backends_run_loop();
}

// handle STRG-C and write config file
void my_handler(int s){
    printf("Caught signal %d\n",s);

	// write config to file
	if (app){
		ofstream myfile;
		myfile.open(X32_CTRL_CONFIGFILE);
		myfile << app->config_to_str(true,true);
		myfile.close();
	}

	// write mixer settings to ini
	ctrl->SaveConfig();

    exit(0); 
}

int main(int argc, char* argv[]) {
	srand(time(NULL));

    Helper* helper = new Helper();
    Config* config = new Config();
    state = new State(helper);

	app = new CLI::App();
	app->description("OpenX32 Main Control");
	argv = app->ensure_utf8(argv);

	// Command line options
	app->set_config("--config", X32_CTRL_CONFIGFILE)
		->configurable(false)
		->description("Load and Store configuration from/to alternate file");;
	app->add_flag("--version", "Get the version number, builddate and a nice logo")
		->configurable(false);
	app->add_flag("-p,--print", "Print configuration and exit")
		->configurable(false);
	app->add_option("--L", "Bitstream file for the Lattice FPGA")
		->option_text("FILE")
		//->default_str("lattice.bit")
		->configurable(false);
	app->add_option("--X", "Bitstream file for the Xilinx FPGA")
		->option_text("FILE")
		//->default_str("xilinx.bit")
		->configurable(false);
	app->add_option("--D1", "Bitstream file for the DSP1")
		->option_text("FILE")
		//->default_str("dsp1.ldr")
		->configurable(false);
	app->add_option("--D2", "Bitstream file for the DSP2")
		->option_text("FILE")
		//->default_str("dsp2.ldr")
		->configurable(false);
	app->add_option("--samplerate", "Set Samplerate to 44100 or 48000 kHz")
		->default_val<uint32_t>(48000)
		->check(CLI::IsMember(new set<uint32_t>{41000, 48000}));

	// debugging commandline option
	vector<string> debug_parameters;
	app->add_option("-d,--debug", debug_parameters, "Prints debugging information to stdout. You can specify one or multiple of the following flags: ADDA DSP1 DSP2 FPGA GUI INI MIXER SPI SURFACE UART VCHANNEL X32CTRL XREMOTE")
			->configurable(false)
			->group("Debug")
			->expected(1,-1)
			->option_text("FLAG FLAG ...");

	bool verbose;
	bool trace;
	const char* catDebug = "Debug";
	const char* catDebugSurface = "Debug Surface";
	app->add_flag("--verbose", verbose, "Print more debug messages")
		->configurable(false)
		->group(catDebug)
		->expected(0,1);

	app->add_flag("--trace", trace, "Print all possible debug messages")
		->configurable(false)
		->group(catDebug)
		->expected(0,1);

	app->add_flag("--timers", state->timers, "Meassure Timers")
		->configurable(false)
		->group(catDebug)
		->expected(0,1);

	app->add_flag("--fpga-spi-speed", state->fpga_spi_speed, "SPI clockrate for bitstream loading and normal data transfer to/from FPGA")
		->configurable(false)
		->group(catDebug)
		->expected(0,1)
		->default_val<uint32_t>(SPI_FPGA_SPEED_HZ);

	app->add_flag("--dsps-spi-config-speed", state->dsp_spi_config_speed, "SPI clockrate for bitstream loading to DSPs")
		->configurable(false)
		->group(catDebug)
		->expected(0,1)
		->default_val<uint32_t>(SPI_DSP_CONF_SPEED_HZ);

	app->add_flag("--dsps-spi-speed", state->dsp_spi_speed, "SPI clockrate for normal data transfer to/from DSPs")
		->configurable(false)
		->group(catDebug)
		->expected(0,1)
		->default_val<uint32_t>(SPI_DSP_SPEED_HZ);

	app->add_flag("--dsps-disable-activity-light", state->dsp_disable_activity_light, "Disable DSPs activity light via SPI switching command")
		->configurable(false)
		->group(catDebug)
		->expected(0,1);

	app->add_flag("--dsps-disable-readout", state->dsp_disable_readout, "Disable DSPs readout")
		->configurable(false)
		->group(catDebug)
		->expected(0,1);

	app->add_flag("--surface-disable-lcd-update", state->surface_disable_lcd_update, "Disable LCD update")
		->configurable(false)
		->group(catDebugSurface)
		->expected(0,1);
	
	app->get_config_formatter_base()->quoteCharacter('"', '"');

	CLI11_PARSE(*app, argc, argv);

	if(app->get_option("--print")->as<bool>()) {
        std::cout << app->config_to_str(true, false);
        return 0;
    }

	if (app->count("--version") > 0) {
		printf("  ____                  __   ______ ___ \n");
		printf(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
		printf("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
		printf("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
		printf("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
		printf(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
		printf("       | |                               \n");
		printf("       |_|                               \n");
		printf("\n");
		printf("%s build on %s %s\n\n%s\n", X32_CTRL_VERSION, __DATE__, __TIME__, X32_CTRL_URL);
		return 0;
	}

	config->SetSamplerate(app->get_option("--samplerate")->as<uint32_t>());

	if (debug_parameters.size() > 0) {
		for(uint8_t i=0; i<debug_parameters.size(); i++) {
			if (debug_parameters[i] == "ADDA") { helper->DEBUG_ADDA(true); }
			if (debug_parameters[i] == "DSP1") { helper->DEBUG_DSP1(true); }
			if (debug_parameters[i] == "DSP2") { helper->DEBUG_DSP2(true); }
			if (debug_parameters[i] == "FPGA") { helper->DEBUG_FPGA(true); }
			if (debug_parameters[i] == "GUI") { helper->DEBUG_GUI(true); }
			if (debug_parameters[i] == "MIXER") { helper->DEBUG_MIXER(true); }
			if (debug_parameters[i] == "SPI") { helper->DEBUG_SPI(true); }
			if (debug_parameters[i] == "SURFACE") { helper->DEBUG_SURFACE(true); }
			if (debug_parameters[i] == "UART") { helper->DEBUG_UART(true); }
			if (debug_parameters[i] == "VCHANNEL") { helper->DEBUG_VCHANNEL(true); }
			if (debug_parameters[i] == "X32CTRL") { helper->DEBUG_X32CTRL(true); }
			if (debug_parameters[i] == "XREMOTE") { helper->DEBUG_XREMOTE(true); }
			if (debug_parameters[i] == "INI") { helper->DEBUG_INI(true); }
			if (debug_parameters[i] == "STATE") { helper->DEBUG_STATE(true); }
		}

		if (trace) {
			helper->SetDebugLevel(DEBUGLEVEL_TRACE);
		} else if (verbose) {
			helper->SetDebugLevel(DEBUGLEVEL_VERBOSE);
		} else {
			helper->SetDebugLevel(DEBUGLEVEL_NORMAL);
		}
	} else {
		helper->SetDebugLevel(DEBUGLEVEL_OFF);
	}

	X32BaseParameter* basepar = new X32BaseParameter(app, config, state, helper);
	ctrl = new X32Ctrl(basepar);

	// handle CTRL+C
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
    
	ctrl->Init();  // initialize the whole thing and load config

	if (config->IsModelX32Core()){
		// only necessary if LVGL is not used
		helper->Log("Starting Timer...\n");
		init100msTimer(); // start 100ms-timer only for Non-GUI systems
		state->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface
		helper->Log("Press Ctrl+C to terminate program.\n");
		while (1) {
			// run service-tasks
			ctrl->Tick10ms();
			// sleep for 10ms to call Tick10ms every ~10ms
			usleep(10000);
		}
	} else {
		helper->Log("Initializing GUI...\n");
		guiInit(); // initializes LVGL, FBDEV and starts endless loop
	}

    exit(0);
}