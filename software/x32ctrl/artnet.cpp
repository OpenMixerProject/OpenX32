/*
	  ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
		     | |                               
		     |_|                               
  
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
*/

#include "artnet.h"

Artnet::Artnet(X32BaseParameter* basepar): X32Base(basepar) {
}

void Artnet::Init() {

  node = artnet_new("0.0.0.0", false); // preferred IP (unsupported for now), verbose
  artnet_set_short_name(node, "OpenX32"); // ShortName
  artnet_set_long_name(node, "OpenX32 ArtNet Node"); // LongName
  artnet_set_node_type(node, ARTNET_RAW); // configure as ArtNET-Server (transmits DMX data)
  
  // install callback-handlers
  //artnet_set_program_handler(node, program_handler, (void*) ops) ;
  //artnet_set_dmx_handler(node, dmx_handler, (void*) ops) ;

  // set first port to input (will transmit data into the network)
  artnet_set_port_type(node, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX); // configure as ArtNET-Transmitter to send DMX data
  artnet_set_port_addr(node, 0, ARTNET_INPUT_PORT, 0); // port = universe
  //artnet_set_port_type(node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX); // configure as ArtNET-Receiver to receive DMX data
  //artnet_set_port_addr(node, 0, ARTNET_OUTPUT_PORT, 0); // port = universe
//  artnet_set_subnet_addr(node, 0);

  if (artnet_start(node) != ARTNET_EOK) {
    helper->Error("Failed to start: %s\n", artnet_strerror() );
  }
}

// this function is called every 50ms and calculates the DMX values to be sent to ArtNet, based on the current channel values and the fade times
void Artnet::Tick() {
  for (uint16_t i = 0; i < 512; i++) {
    if (dmxDest[i] > dmx[i]) {
      dmx[i] += dmxStep[i];
    }else if (dmxDest[i] < dmx[i]) {
      dmx[i] -= dmxStep[i];
    }

    // copy new data to output-array
    //dmxOutput[i] = dmx[i];

    // debug-data on Channel 1 to 10
    if ((i >= 0) && (i < 10)) {
      dmxOutput[i] += 1;
    }
  }

  // send on configured ports
  artnet_send_dmx(node, 0, 512, &dmxOutput[0]); // artnet-node, univese, length, data

  // allow sending even on unconfigured ports for testing
  //artnet_raw_send_dmx(node, 1, 512, &dmxOutput[0]); // artnet-node, univese, length, data

  // read incoming data and answer to ArtPollRequest with an ArtPollReply
  artnet_read(node, 0);
}

void Artnet::setChannel(uint16_t channel, float value, float timeMs) {
  float delta = value - dmx[channel];

  if (timeMs <= 0) {
    // no fading, set value directly
    dmx[channel] = value;
    dmxDest[channel] = value;
    dmxStep[channel] = 0;
    return;
  }else{
    // calculate fading
    dmxDest[channel] = value;
    dmxStep[channel] = delta / (timeMs / 50.0f); // calculate the step to reach the target value in the given time, based on the 50ms tick-rate
  }
}

