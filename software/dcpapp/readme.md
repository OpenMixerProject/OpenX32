# Overview
The dcp-compiler converts one or more binary-files compiled for the i.MX25 to be loaded and executed on booting up of the X32.

## Option A: Run from RAM
Use the file-extension ".run" to execute the files directly from RAM without changing the content of the SD-Card. OpenX32 can be used without any limitations, but the original operating system will start after a power-cycle.

## Option B: Firmware-Update
Use the file-extension ".update" to copy the binary to the internal SD-Card. This will permanently install OpenX32 on your console.

## Usage
    perl dcp_compiler.pl openx32.bin:binary/dcpapp.bin dcp_corefs_openx32.run
	
## Credits
Published with the kind permission of T. Zint from Behringer/MusicGroup. Thank you!