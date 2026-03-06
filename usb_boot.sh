#!/bin/bash
echo "Uploading OpenX32 via Serial Download Protocol (USB-OTG) to RAM at Address 0x80000000..."
cd pyatk
source pyatk_venv/bin/activate
cd bin
# sudo python3 mx-toolkit.py listbsp
sudo ../pyatk_venv/bin/python3 mx-toolkit.py run -b mx25 /tmp/openx32.bin 0x80000000 -i meminit.txt
deactivate
echo "Done."
