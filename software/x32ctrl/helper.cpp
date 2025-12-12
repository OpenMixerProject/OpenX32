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

#include "helper.h"

void Helper::Log(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	vprintf(format, args);
	fflush(stdout); // immediately write to console!

	va_end(args);
}

void Helper::Error(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	vprintf((String("ERROR: ") + String(format)).c_str(), args);
	fflush(stdout); // immediately write to console!

	va_end(args);
}

unsigned int Helper::Checksum(char* str) {
   unsigned int sum = 0;
   while (*str) {
	  sum += *str;
	  str++;
   } 
   return sum;
}

int Helper::ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size) {
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

float Helper::Fadervalue2dBfs(uint16_t faderValue) {
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

uint16_t Helper::Oscvalue2Fadervalue(float oscValue){
	return (uint16_t)oscValue * 0x0FFF;
}

float Helper::Fadervalue2Oscvalue(uint16_t faderValue){
	return (float)faderValue / (float)0x0FFF;
}

float Helper::Dbfs2Oscvalue(float dbfsValue) {
	return Fadervalue2Oscvalue(Dbfs2Fader(dbfsValue));
}

float Helper::samplePu2Dbfs(float samplePu) {
    if (samplePu < (1.0f / 2147483648.0f)) {
        return -120.0f;
    }

    return (20.0f * log10f(samplePu));
}

float Helper::sample2Dbfs(uint32_t sample) {
    if (sample < 1) {
        return -120;
    }

    //return (-6 * (__builtin_clz(sample) - 1));
    return (20.0f * log10f((float)sample/2147483648.0f));
}

uint16_t Helper::Dbfs2Fader(float dbfsValue) {
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

long Helper::GetFileSize(const char* filename) {
	struct stat st;
	if (stat(filename, &st) == 0) {
		return st.st_size;
	}
	return -1; // error
}

void Helper::ReverseBitOrderArray(uint8_t* data, uint32_t len) {
	// reverse bits in array
	uint32_t* pData = (uint32_t*)data;
	for (uint32_t i = 0; i < (len/4); i++) {
		*pData = ReverseBitOrder_uint32(*pData);
		pData++;
	}
}

uint32_t Helper::ReverseBitOrder_uint32(uint32_t n) {
	n = ((n >> 1) & 0x55555555) | ((n & 0x55555555) << 1);
	n = ((n >> 2) & 0x33333333) | ((n & 0x33333333) << 2);
	n = ((n >> 4) & 0x0F0F0F0F) | ((n & 0x0F0F0F0F) << 4);
	n = ((n >> 8) & 0x00FF00FF) | ((n & 0x00FF00FF) << 8);
	n = (n >> 16) | (n << 16);
	return n;
}

// saturates a float-value to a specific minimum and maximum value
float Helper::Saturate(float value, float min, float max) {
  if (value>max) {
	return max;
  }else if (value<min) {
	return min;
  }else{
	return value;
  }
}

String Helper::freq2String(float freq) {
  if (freq < 1000) {
	return String(freq, 1) + " Hz";
  }else{
	return String(freq / 1000.0f, 1) + " kHz";
  };
}

String Helper::eqType2String(uint8_t type) {
  // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
  switch (type) {
	case 0:
	  return "Allpass";
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

String Helper::getIpAddress() {
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

bool Helper::IsInChannelBlock(uint8_t index, X32_VCHANNEL_BLOCK block){
  uint8_t blocksize = 0;
  switch(block){
		case X32_VCHANNEL_BLOCK_NORMAL: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_NORMAL;
			break;
		}
	case X32_VCHANNEL_BLOCK_AUX: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_AUX;
			break;
		}
	case X32_VCHANNEL_BLOCK_FXRET: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_FXRET;
			break;
		}
	case X32_VCHANNEL_BLOCK_BUS: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_BUS;
			break;
		}
	case X32_VCHANNEL_BLOCK_MATRIX: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_MATRIX;
			break;
		}
	case X32_VCHANNEL_BLOCK_SPECIAL: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_SPECIAL;
			break;
		}
	case X32_VCHANNEL_BLOCK_MAINSUB: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_MAINSUB;
			break;
		}
	case X32_VCHANNEL_BLOCK_DCA: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_DCA;
			break;
		}
	case X32_VCHANNEL_BLOCK_MAIN: {
	  blocksize = X32_VCHANNEL_BLOCKSIZE_MAIN;
			break;
		}
	}
  return (index >= block) && (index < (block + blocksize));
}

vector<string> Helper::split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
