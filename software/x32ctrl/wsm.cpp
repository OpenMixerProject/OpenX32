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
  

  This file offers a server for the XRemote-protocol

  The XRemote-protocol is used to communicate with the XEdit software.
  The communication is based on OSC-protocol.

  Parts of this file are based on the "Unofficial X32/M32 OSC Remote Protocol" v4.02 by Patrick-Gilles Maillot.
  Thank you very much for sharing your work!

  Linux UDP examples:
  - https://gist.github.com/saxbophone/f770e86ceff9d488396c0c32d47b757e
  - https://openbook.rheinwerk-verlag.de/linux_unix_programmierung/Kap11-016.htm
*/

#include "wsm.h"

WSM::WSM(X32BaseParameter* basepar) : X32Base(basepar) {}

int WSM::Init()
{ 
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(53212);

    if ((UdpHandle = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        //fprintf(stderr, "Error on creating UDP-socket!");
        return -1;
    }
    
    if (bind(UdpHandle, (const struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0) {
        //fprintf(stderr, "Error on binding UDP-socket!");
        close(UdpHandle);
        return -1;
    }
    
    // debug
    //AddClient("192.168.188.109");

    return 0;
}

void WSM::RequestDataFromClients()
{
    requestWaitCounter++;

    if (requestWaitCounter % 10 == 0)
    {
        for (int i = 0; i < clients.size(); i++)
        {
            SendCommand("Push 0 400 1\r", clients.at(i));
        }
    }
}

void WSM::AddClient(String ip)
{
    clients.push_back(ip);
}

////////////////////////////////////
// low-level communication functions
////////////////////////////////////

void WSM::SendCommand(String command, String ip)
{
    struct sockaddr_in Addr;
    Addr.sin_family = AF_INET;
    inet_aton(ip.c_str(), &Addr.sin_addr);
    Addr.sin_port = htons(53212);

    sendto(
        UdpHandle,
        (char*)command.c_str(),
        command.length(),
        0,
        (struct sockaddr *) &Addr,
        sizeof(Addr)
    );
}