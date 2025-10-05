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

    x32log("  ____                  __   ______ ___  \n");
    x32log(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
    x32log("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
    x32log("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
    x32log("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
    x32log(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
    x32log("       | |                               \n");
    x32log("       |_|                               \n");
    x32log("OpenX32 Main Control\n");
    x32log("v0.1.5, 30.09.2025\n");
    x32log("https://github.com/OpenMixerProject/OpenX32\n");

    // first try to find what we are: Fullsize, Compact, Producer, Rack or Core
    x32debug("Reading config...\n");
    char model[12];
    char serial[15];
    char date[16];
    readConfig("/etc/x32.conf", "MDL=", model, 12);
    readConfig("/etc/x32.conf", "SN=", serial, 15);
    readConfig("/etc/x32.conf", "DATE=", date, 16);
    x32log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

    X32Config* config = new X32Config();
    config->SetModel(model);
    config->SetSamplerate(48000);

    Mixer* mixer = new Mixer();

    cout << "";
    

    return 0;
}
