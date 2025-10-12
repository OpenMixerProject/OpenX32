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

  Control software for Behringer X32 using OpenX32
  https://github.com/OpenMixerProject/OpenX32
  v0.1.5, 30.09.2025

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

#include "x32ctrl.h"


// the main-function - of course
int main(int argc, char* argv[]) {
    srand(time(NULL));
    Config* config = new Config();
    Helper helper(config);

    helper.Log("  ____                  __   ______ ___  \n");
    helper.Log(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
    helper.Log("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
    helper.Log("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
    helper.Log("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
    helper.Log(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
    helper.Log("       | |                               \n");
    helper.Log("       |_|                               \n");
    helper.Log("OpenX32 Main Control\n");
    helper.Log("v0.2.0, 12.10.2025\n");
    helper.Log("https://github.com/OpenMixerProject/OpenX32\n");

    config->SetDebug(false);

    // first try to find what we are: Fullsize, Compact, Producer, Rack or Core
    helper.Debug("Reading config...\n");
    char model[12];
    char serial[15];
    char date[16];
    helper.ReadConfig("/etc/x32.conf", "MDL=", model, 12);
    helper.ReadConfig("/etc/x32.conf", "SN=", serial, 15);
    helper.ReadConfig("/etc/x32.conf", "DATE=", date, 16);
    helper.Log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

    
    config->SetModel(model);
    config->SetSamplerate(48000);

    Mixer* mixer = new Mixer(config);

    // check start-switches
    int8_t switchFpga = -1;
    int8_t switchDsp1 = -1;
    int8_t switchDsp2 = -1;
    int8_t switchNoinit = -1;
    parseParams(argc, argv, &switchFpga, &switchDsp1, &switchDsp2, &switchNoinit);
    // // initializing DSPs and FPGA
    // if (switchFpga > 0) { spiConfigureFpga(argv[switchFpga]); }
    // if ((switchDsp1 > 0) && (switchDsp2 == -1)) { spiConfigureDsp(argv[switchDsp1], "", 1); }
    // if ((switchDsp1 > 0) && (switchDsp2 > 0)) { spiConfigureDsp(argv[switchDsp1], argv[switchDsp2], 2); }



    while(1){
      usleep(10 * 1000);
        mixer->Tick10ms();
    }

    return 0;
}


void parseParams(int argc, char* argv[], int8_t* fpga, int8_t* dsp1, int8_t* dsp2, int8_t* noinit) {
    for (int8_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-fpga") == 0) {
            if (i + 1 < argc) {
                *fpga = i+1;
                i++;
            } else {
                *fpga = -1;
            }
        }
        else if (strcmp(argv[i], "-dsp1") == 0) {
            if (i + 1 < argc) {
                *dsp1 = i+1;
                i++;
            } else {
                *dsp1 = -1;
            }
        }
        else if (strcmp(argv[i], "-dsp2") == 0) {
            if (i + 1 < argc) {
                *dsp2 = i+1;
                i++;
            } else {
                *dsp2 = -1;
            }
        }
        else if (strcmp(argv[i], "-noinit") == 0) {
            if (i + 1 < argc) {
                *noinit = i+1;
                i++;
            } else {
                *noinit = -1;
            }
        }
        // handle unknown parameters
        else {
            printf("Unknown parameter: %s\n", argv[i]);
        }
    }
}
