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

#include "auxiliary.h"

int readConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size) {
    FILE *file = NULL;
    char line[64];
    int found = -2;

    // open file
    file = fopen(filename, "r");
    if (file == NULL) {
        // no configuration-file
        strncpy(value_buffer, "Mysterious", buffer_size);
        return -1;
    }

    size_t key_len = strlen(key);
    char search_pattern[key_len + 2];
    snprintf(search_pattern, sizeof(search_pattern), "%s", key);

    // read file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            char* value_start = line + strlen(search_pattern);

            // remove spaces (if any)
            while (*value_start == ' ' || *value_start == '\t') {
                value_start++;
            }

            // remove unwanted characters from the end
            char* newline_pos = strchr(value_start, '\n');
            if (newline_pos != NULL) {
                *newline_pos = '\0';
            }
            char* cr_pos = strchr(value_start, '\r');
            if (cr_pos != NULL) {
                *cr_pos = '\0';
            }

            // copy value to buffer
            size_t value_len = strlen(value_start);
            if (value_len >= buffer_size) {
                found = -3; // buffer to small
                break;
            }
            strncpy(value_buffer, value_start, buffer_size - 1);
            value_buffer[buffer_size - 1] = '\0';
            found = 0; // success
            break; // value found. End loop
        }
    }
    fclose(file);

	return found;
}

// saturates a value to a specific minimum value
float limitMin(float value, float min) {
  if (value<min) {
    return min;
  }else{
    return value;
  }
}

// saturates a value to a specific maximum value
float limitMax(float value, float max) {
  if (value>max) {
    return max;
  }else{
    return value;
  }
}

// saturates a float-value to a specific minimum and maximum value
float saturate(float value, float min, float max) {
  if (value>max) {
    return max;
  }else if (value<min) {
    return min;
  }else{
    return value;
  }
}

float fader2dBfs(uint16_t faderValue) {
  // X32 uses three different scales on fader. This function converters
  // the linear RAW-fader-value into a dBfs value matching the silk-screen
  // of the faders
  const uint16_t FADER_MAX = 4095;
  //const uint16_t FADER_0DB = (uint16_t)(0.75 * FADER_MAX);  // unused
  const uint16_t FADER_MINUS_10DB = (uint16_t)(0.5 * FADER_MAX);
  const uint16_t FADER_MINUS_30DB = (uint16_t)(0.25 * FADER_MAX);
  const uint16_t FADER_MINUS_INFDB = 0;

  if (faderValue <= FADER_MINUS_INFDB) {
      return VOLUME_MIN;
  }

  // Group A: +10 dBfs to -10 dBfs (Fader from 4095 to FADER_MINUS_10DB)
  if (faderValue >= FADER_MINUS_10DB) {
    // linear interpolation between -10 dBfs and +10 dBfs
    // Range: 20 dBfs
    float range_fader = FADER_MAX - FADER_MINUS_10DB;
    float position_fader = faderValue - FADER_MINUS_10DB;
    float interpolated_dbfs = (position_fader / range_fader) * 20.0 - 10.0;
    return interpolated_dbfs;
  }
  // Group B: -10 dBfs to -30 dBfs (Fader from FADER_MINUS_10DB to FADER_MINUS_30DB)
  else if (faderValue > FADER_MINUS_30DB && faderValue < FADER_MINUS_10DB) {
    // linear interpolation between -30 dBfs and -10 dBfs
    // Range: 20 dBfs
    float range_fader = FADER_MINUS_10DB - FADER_MINUS_30DB;
    float position_fader = faderValue - FADER_MINUS_30DB;
    float interpolated_dbfs = (position_fader / range_fader) * 20.0 - 30.0;
    return interpolated_dbfs;
  }
  // Group C: -30 dBfs to -70 dBfs (-inf) (Fader from FADER_MINUS_30DB to FADER_MINUS_INFDB)
  else {
    // linear interpolation between -70 dBfs and -30 dBfs
    // Range: 40 dBfs
    float range_fader = FADER_MINUS_30DB - FADER_MINUS_INFDB;
    float position_fader = faderValue - FADER_MINUS_INFDB;
    float interpolated_dbfs = (position_fader / range_fader) * 40.0 - 70.0;
    return interpolated_dbfs;
  }
}

uint16_t dBfs2fader(float dbfsValue) {
  const uint16_t FADER_MAX = 4095;
  //const uint16_t FADER_0DB = (uint16_t)(0.75 * FADER_MAX); // unused
  const uint16_t FADER_MINUS_10DB = (uint16_t)(0.5 * FADER_MAX);
  const uint16_t FADER_MINUS_30DB = (uint16_t)(0.25 * FADER_MAX);
  const uint16_t FADER_MINUS_INFDB = 0;

  // Group A: +10 dBfs to -10 dBfs
  if (dbfsValue >= -10.0f) {
    // Interpolate between FADER_MINUS_10DB and FADER_MAX
    // dBfs range: -10 to +10 (20 dB)
    float normalized_dbfs = (dbfsValue + 10.0f) / 20.0f;
    uint16_t fader_value = FADER_MINUS_10DB + (uint16_t)(normalized_dbfs * (FADER_MAX - FADER_MINUS_10DB));
    if (fader_value > FADER_MAX) return FADER_MAX;
    return fader_value;
  }
  // Group B: -10 dBfs to -30 dBfs
  else if (dbfsValue >= -30.0f) {
    // Interpolate between FADER_MINUS_30DB and FADER_MINUS_10DB
    // dBfs range: -30 to -10 (20 dB)
    float normalized_dbfs = (dbfsValue + 30.0f) / 20.0f;
    uint16_t fader_value = FADER_MINUS_30DB + (uint16_t)(normalized_dbfs * (FADER_MINUS_10DB - FADER_MINUS_30DB));
    if (fader_value > FADER_MINUS_10DB) return FADER_MINUS_10DB -1; // -1 to avoid crossing into the next group
    return fader_value;
  }
  // Group C: -30 dBfs to -70 dBfs (-inf)
  else {
    // Interpolate between FADER_MINUS_INFDB and FADER_MINUS_30DB
    // dBfs range: -70 to -30 (40 dB)
    if (dbfsValue <= -70.0f) {
      return FADER_MINUS_INFDB;
    }
    float normalized_dbfs = (dbfsValue + 70.0f) / 40.0f;
    uint16_t fader_value = FADER_MINUS_INFDB + (uint16_t)(normalized_dbfs * (FADER_MINUS_30DB - FADER_MINUS_INFDB));
    if (fader_value > FADER_MINUS_30DB) return FADER_MINUS_30DB-1;
    return fader_value;
  }
}

String getIpAddress() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if ((!ifa->ifa_addr) || (String(ifa->ifa_name) == "lo")) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) {
            // check if its a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            return addressBuffer;
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
/*
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
*/
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    return "";
}

/*
int valueToBinaryString(uint8_t value, char* output) {
    if (value > 15) {
        return -1;
    }

    // zero-terminate string
    output[4] = '\0';

    for (int i = 3; i >= 0; --i) {
        if ((value >> i) & 1) {
            output[3 - i] = '1';
        } else {
            output[3 - i] = '0';
        }
    }

    return 0;
}
*/

// variables
timer_t timerid;
struct sigevent sev;
struct itimerspec trigger;
sigset_t mask;

// initialize 100ms timer (only for Non-GUI systems)
int init100msTimer() {
  // Set up the signal handler
  struct sigaction sa;
  sa.sa_handler = timer100msCallbackLinux;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  // Set up the sigevent structure for the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;

  // Create the timer
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
    perror("timer_create");
    return 1;
  }

  // Set the timer to trigger every 1 second (1,000,000,000 nanoseconds)
  trigger.it_value.tv_sec = 0;
  trigger.it_value.tv_nsec = 50000000; // 50ms = 50000us = 50000000ns
  trigger.it_interval.tv_sec = 0;
  trigger.it_interval.tv_nsec = 50000000;

  // Arm the timer
  if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
    perror("timer_settime");
    return 1;
  }
}

long getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1; // error
}

uint32_t reverseBitOrder_uint32(uint32_t n) {
    n = ((n >> 1) & 0x55555555) | ((n & 0x55555555) << 1);
    n = ((n >> 2) & 0x33333333) | ((n & 0x33333333) << 2);
    n = ((n >> 4) & 0x0F0F0F0F) | ((n & 0x0F0F0F0F) << 4);
    n = ((n >> 8) & 0x00FF00FF) | ((n & 0x00FF00FF) << 8);
    n = (n >> 16) | (n << 16);
    return n;
}

void reverseBitOrderArray(uint8_t* data, uint32_t len) {
    // reverse bits in array
    uint32_t* pData = (uint32_t*)data;
    for (uint32_t i = 0; i < (len/4); i++) {
        *pData = reverseBitOrder_uint32(*pData);
        pData++;
    }
}

String freq2String(float freq) {
  if (freq < 1000) {
    return String(freq, 1) + " Hz";
  }else{
    return String(freq / 1000.0f, 1) + " kHz";
  };
}

String eqType2String(uint8_t type) {
  // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
  switch (type) {
    case 0:
      return "Allp";
      break;
    case 1:
      return "PEQ";
      break;
    case 2:
      return "LShv";
      break;
    case 3:
      return "HShv";
      break;
    case 4:
      return "Bandp";
      break;
    case 5:
      return "Notch";
      break;
    case 6:
      return "HCut";
      break;
    case 7:
      return "LCut";
      break;
    default:
      return "???";
      break;
  }
}