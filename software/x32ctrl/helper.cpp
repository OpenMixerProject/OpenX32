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

Helper::Helper() {
	InitMixerparameters();
}

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


void Helper::starttimer(uint8_t timer) {
	plf::nanotimer* t = new plf::nanotimer();
	timers[timer] = t;
	t->start();
}

void Helper::stoptimer(uint8_t timer, const char* description) {

	if (timers[timer] == NULL)
	{
		return;
	}

	plf::nanotimer* t = timers.at(timer);
	double results = t->get_elapsed_ms();
	
	timers.erase(timer);
	delete(t);

	printf("Timing %s: %fms\n", description, results);
}

uint8_t Helper::value2percent(float value, float value_min, float value_max) {
	float onehunderedpercent = value_max - value_min;
	float value_normiert = value - value_min;
	float onepercent = onehunderedpercent / 100.0f;
	return value_normiert / onepercent;
}

uint8_t Helper::value2percent(uint8_t value, uint8_t value_min, uint8_t value_max) {
	uint8_t onehunderedpercent = value_max - value_min;
	uint8_t value_normiert = value - value_min;
	float onepercent = onehunderedpercent / 100.0f;
	return value_normiert / onepercent;
}

uint8_t Helper::value2percent(int8_t value, int8_t value_min, int8_t value_max) {
	uint8_t onehunderedpercent = value_max - value_min;
	uint8_t value_normiert = value - value_min;
	float onepercent = onehunderedpercent / 100.0f;
	return value_normiert / onepercent;
}


// ##     ## #### ##      ## ######## ########  ########     ###    ########     ###    ##     ## ######## ######## ######## ########  
// ###   ###  ##   ##    ##  ##       ##     ## ##     ##   ## ##   ##     ##   ## ##   ###   ### ##          ##    ##       ##     ## 
// #### ####  ##    ##  ##   ##       ##     ## ##     ##  ##   ##  ##     ##  ##   ##  #### #### ##          ##    ##       ##     ## 
// ## ### ##  ##     ####    ######   ########  ########  ##     ## ########  ##     ## ## ### ## ######      ##    ######   ########  
// ##     ##  ##    ##  ##   ##       ##   ##   ##        ######### ##   ##   ######### ##     ## ##          ##    ##       ##   ##   
// ##     ##  ##   ##    ##  ##       ##    ##  ##        ##     ## ##    ##  ##     ## ##     ## ##          ##    ##       ##    ##  
// ##     ## #### ##      ## ######## ##     ## ##        ##     ## ##     ## ##     ## ##     ## ########    ##    ######## ##     ## 



String Helper::GetUnitOfMesaurement(MixerparameterDefinition mpd, bool spaceInFront = false) {
	
	String result = spaceInFront ? " " : "";

	switch(mpd.unitOfMeasurement)
		{
		case MIXERPARAMETER_UOM_DB:
			result += "dB";
			break;
		case MIXERPARAMETER_UOM_DBFS:
			result += "dbFS";
			break;
		case MIXERPARAMETER_UOM_HZ:
			result += "Hz";
			break;
		case MIXERPARAMETER_UOM_KHZ:
			result += "kHz";
			break;
		case MIXERPARAMETER_UOM_MS:
			result += "ms";
			break;
		default:
			result += "";
	}	

	return result;
}

float Helper::CalcNewValue(float value, int8_t amount, MIXERPARAMETER mp) {
	MixerparameterDefinition mpd = GetMixerparameterDefinition(mp);

	float newValue = value + (amount * mpd.float_stepsize);

	if (newValue > mpd.float_value_max) {
		newValue = mpd.float_value_max;
	} else if (newValue < mpd.float_value_min) {
		newValue = mpd.float_value_min;
	}
	
	return newValue;
}

String Helper::FormatValue(float value, MixerparameterDefinition mpd) {
	return String(value, mpd.GetDecimaPlaces()) + GetUnitOfMesaurement(mpd, true);
}

String Helper::FormatValue(uint8_t value, MixerparameterDefinition mpd) {
	return String(value) + GetUnitOfMesaurement(mpd, true);
}

String Helper::FormatValue(int8_t value, MixerparameterDefinition mpd) {
	return String(value) + GetUnitOfMesaurement(mpd, true);
}


MixerparameterDefinition Helper::GetMixerparameterDefinition(MIXERPARAMETER mp){
    return mpm[mp];
}

void Helper::InitMixerparameters() {

    // generic
    mpm[MIXERPARAMETER_LCD_CONTRAST] = MixerparameterDefinition("LCD Contrast", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_LCD_CONTRAST, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_LCD_CONTRAST].SetMinMaxStandard_Uint8(LCD_CONTRAST_MIN, LCD_CONTRAST_MAX, LCD_CONTRAST_DEFAULT); 

    mpm[MIXERPARAMETER_LED_BRIGHTNESS] = MixerparameterDefinition("LED Brightness", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_LED_BRIGHTNESS, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_LED_BRIGHTNESS].SetMinMaxStandard_Uint8(LED_BRIGHTNESS_1, LED_BRIGHTNESS_4, LED_BRIGHTNESS_4); 

    // channel
    mpm[MIXERPARAMETER_CHANNEL_GAIN] = MixerparameterDefinition("Gain", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_NONE, X32_VCHANNEL_CHANGED_GAIN, true);
    mpm[MIXERPARAMETER_CHANNEL_GAIN].SetMinMaxStandardDecimals_Float(CHANNEL_GAIN_MIN, CHANNEL_GAIN_MAX, 0.0f, 1);
    mpm[MIXERPARAMETER_CHANNEL_PANORAMA] = MixerparameterDefinition("Pan/Bal", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_BALANCE,true);
    mpm[MIXERPARAMETER_CHANNEL_PANORAMA].SetMinMaxStandardDecimals_Float(CHANNEL_PANORAMA_MIN, CHANNEL_PANORAMA_MAX, 0.0f, 0);
    mpm[MIXERPARAMETER_CHANNEL_VOLUME] = MixerparameterDefinition("Volume", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_VOLUME, true);
    mpm[MIXERPARAMETER_CHANNEL_VOLUME].SetMinMaxStandardDecimals_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);

    // gate
    mpm[MIXERPARAMETER_GATE_TRESHOLD] = MixerparameterDefinition("Threshold", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_GATE, true);
    mpm[MIXERPARAMETER_GATE_TRESHOLD].SetMinMaxStandardDecimals_Float(GATE_THRESHOLD_MIN, GATE_THRESHOLD_MAX, GATE_THRESHOLD_MIN, 0);
    mpm[MIXERPARAMETER_GATE_RANGE] = MixerparameterDefinition("Range", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_GATE, true);
    mpm[MIXERPARAMETER_GATE_RANGE].SetMinMaxStandardDecimals_Float(GATE_RANGE_MIN, GATE_RANGE_MAX, GATE_RANGE_MAX, 1);
    mpm[MIXERPARAMETER_GATE_ATTACK] = MixerparameterDefinition("Attack", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_GATE, true);
    mpm[MIXERPARAMETER_GATE_ATTACK].SetMinMaxStandardDecimals_Float(GATE_ATTACK_MIN, GATE_ATTACK_MAX, 10.0f, 0);
    mpm[MIXERPARAMETER_GATE_HOLD] = MixerparameterDefinition("Hold", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_GATE, true);
    mpm[MIXERPARAMETER_GATE_HOLD].SetMinMaxStandardDecimals_Float(GATE_HOLD_MIN, GATE_HOLD_MAX, 50.0f, 0);
    mpm[MIXERPARAMETER_GATE_RELEASE] = MixerparameterDefinition("Release", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_GATE, true);
    mpm[MIXERPARAMETER_GATE_RELEASE].SetMinMaxStandardDecimals_Float(GATE_RELEASE_MIN, GATE_RELEASE_MAX, 250.0f, 0);
    
    // dynamics
    mpm[MIXERPARAMETER_DYNAMICS_TRESHOLD] = MixerparameterDefinition("Threshold", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_TRESHOLD].SetMinMaxStandardDecimals_Float(DYNAMICS_THRESHOLD_MIN, DYNAMICS_THRESHOLD_MAX, DYNAMICS_THRESHOLD_MAX, 0);
    mpm[MIXERPARAMETER_DYNAMICS_RATIO] = MixerparameterDefinition("Ratio", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_RATIO].SetMinMaxStandardDecimals_Float(DYNAMICS_RATIO_MIN, DYNAMICS_RATIO_MAX, 3, 1);
    mpm[MIXERPARAMETER_DYNAMICS_MAKEUP] = MixerparameterDefinition("Makeup", MIXERPARAMETER_UOM_DB, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_MAKEUP].SetMinMaxStandardDecimals_Float(DYNAMICS_MAKEUP_MIN, DYNAMICS_MAKEUP_MAX, DYNAMICS_MAKEUP_MIN, 1);
    mpm[MIXERPARAMETER_DYNAMICS_ATTACK] = MixerparameterDefinition("Attack", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_ATTACK].SetMinMaxStandardDecimals_Float(DYNAMICS_ATTACK_MIN, DYNAMICS_ATTACK_MAX, 10.0f, 0);
    mpm[MIXERPARAMETER_DYNAMICS_HOLD] = MixerparameterDefinition("Hold", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_HOLD].SetMinMaxStandardDecimals_Float(DYNAMICS_HOLD_MIN, DYNAMICS_HOLD_MAX, 10.0f, 0);
    mpm[MIXERPARAMETER_DYNAMICS_RELEASE] = MixerparameterDefinition("Release", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_VCHANNEL, X32_VCHANNEL_CHANGED_DYNAMIC, true);
    mpm[MIXERPARAMETER_DYNAMICS_RELEASE].SetMinMaxStandardDecimals_Float(DYNAMICS_RELEASE_MIN, DYNAMICS_RELEASE_MAX, 150.0f, 0);

    // FX

    // reverb
    #define FX_REVERB_ROOMSIZE_MIN         0.0f // ms
    #define FX_REVERB_ROOMSIZE_DEFAULT   150.0f // ms
    #define FX_REVERB_ROOMSIZE_MAX      1000.0f // ms
    #define FX_REVERB_RT60_MIN             0.0f // s
    #define FX_REVERB_RT60_DEFAULT         3.0f // s
    #define FX_REVERB_RT60_MAX           100.0f // s
    #define FX_REVERB_LPFREQ_MIN           0.0f // Hz
    #define FX_REVERB_LPFREQ_DEFAULT   14000.0f // Hz
    #define FX_REVERB_LPFREQ_MAX       20000.0f // Hz
    #define FX_REVERB_DRY_MIN              0.0f //
    #define FX_REVERB_DRY_DEFAULT          1.0f //
    #define FX_REVERB_DRY_MAX              1.0f //
    #define FX_REVERB_WET_MIN              0.0f //
    #define FX_REVERB_WET_DEFAULT          0.25f //
    #define FX_REVERB_WET_MAX              1.0f //

    // reverb roomsize
    mpm[MIXERPARAMETER_FX_REVERB_ROOMSIZE] = MixerparameterDefinition("Roomsize", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_REVERB_ROOMSIZE].SetMinMaxStandardDecimals_Float(FX_REVERB_ROOMSIZE_MIN, FX_REVERB_ROOMSIZE_MAX, FX_REVERB_ROOMSIZE_DEFAULT, 0);
    // reverb rt60
    mpm[MIXERPARAMETER_FX_REVERB_RT60] = MixerparameterDefinition("RT60", MIXERPARAMETER_UOM_S, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_REVERB_RT60].SetMinMaxStandardDecimals_Float(FX_REVERB_RT60_MIN, FX_REVERB_RT60_MAX, FX_REVERB_RT60_DEFAULT, 1);
    // reverb lowpass
    mpm[MIXERPARAMETER_FX_REVERB_LPFREQ] = MixerparameterDefinition("LowPass", MIXERPARAMETER_UOM_KHZ, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_REVERB_LPFREQ].SetMinMaxStandardDecimals_Float(FX_REVERB_LPFREQ_MIN, FX_REVERB_LPFREQ_MAX, FX_REVERB_LPFREQ_DEFAULT, 0);
    // reverb dry
    mpm[MIXERPARAMETER_FX_REVERB_DRY] = MixerparameterDefinition("Dry", MIXERPARAMETER_UOM_PERCENT, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_REVERB_DRY].SetMinMaxStandardDecimals_Float(FX_REVERB_DRY_MIN, FX_REVERB_DRY_MAX, FX_REVERB_DRY_DEFAULT, 2);
    // reverb wet
    mpm[MIXERPARAMETER_FX_REVERB_WET] = MixerparameterDefinition("Wet", MIXERPARAMETER_UOM_PERCENT, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_REVERB_WET].SetMinMaxStandardDecimals_Float(FX_REVERB_WET_MIN, FX_REVERB_WET_MAX, FX_REVERB_WET_DEFAULT, 2);

    // chorus
    #define FX_CHORUS_DEPTH_A_MIN          0.0f //
    #define FX_CHORUS_DEPTH_A_DEFAULT     10.0f //
    #define FX_CHORUS_DEPTH_A_MAX        100.0f //
    #define FX_CHORUS_DEPTH_B_MIN          0.0f //
    #define FX_CHORUS_DEPTH_B_DEFAULT     10.0f //
    #define FX_CHORUS_DEPTH_B_MAX        100.0f //
    #define FX_CHORUS_DELAY_A_MIN          0.0f // ms
    #define FX_CHORUS_DELAY_A_DEFAULT     15.0f // ms
    #define FX_CHORUS_DELAY_A_MAX        100.0f // ms
    #define FX_CHORUS_DELAY_B_MIN          0.0f // ms
    #define FX_CHORUS_DELAY_B_DEFAULT     20.0f // ms
    #define FX_CHORUS_DELAY_B_MAX        100.0f // ms
    #define FX_CHORUS_PHASE_A_MIN          0.0f //
    #define FX_CHORUS_PHASE_A_DEFAULT      0.0f //
    #define FX_CHORUS_PHASE_A_MAX        100.0f //
    #define FX_CHORUS_PHASE_B_MIN          0.0f //
    #define FX_CHORUS_PHASE_B_DEFAULT      0.0f //
    #define FX_CHORUS_PHASE_B_MAX        100.0f //
    #define FX_CHORUS_FREQ_A_MIN           0.0f //
    #define FX_CHORUS_FREQ_A_DEFAULT       1.5f //
    #define FX_CHORUS_FREQ_A_MAX         100.0f //
    #define FX_CHORUS_FREQ_B_MIN           0.0f //
    #define FX_CHORUS_FREQ_B_DEFAULT       1.6f //
    #define FX_CHORUS_FREQ_B_MAX         100.0f //
    #define FX_CHORUS_MIX_MIN              0.0f //
    #define FX_CHORUS_MIX_DEFAULT          0.5f //
    #define FX_CHORUS_MIX_MAX              1.0f //

    // chorus depth
    mpm[MIXERPARAMETER_FX_CHORUS_DEPTH_A] = MixerparameterDefinition("Depth A", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_DEPTH_A].SetMinMaxStandardDecimals_Float(FX_CHORUS_DEPTH_A_MIN, FX_CHORUS_DEPTH_A_MAX, FX_CHORUS_DEPTH_A_DEFAULT, 0);
    mpm[MIXERPARAMETER_FX_CHORUS_DEPTH_B] = MixerparameterDefinition("Depth B", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_DEPTH_B].SetMinMaxStandardDecimals_Float(FX_CHORUS_DEPTH_B_MIN, FX_CHORUS_DEPTH_B_MAX, FX_CHORUS_DEPTH_B_DEFAULT, 0);
    // chorus delay
    mpm[MIXERPARAMETER_FX_CHORUS_DELAY_A] = MixerparameterDefinition("Delay A", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_DELAY_A].SetMinMaxStandardDecimals_Float(FX_CHORUS_DELAY_A_MIN, FX_CHORUS_DELAY_A_MAX, FX_CHORUS_DELAY_A_DEFAULT, 0);
    mpm[MIXERPARAMETER_FX_CHORUS_DELAY_B] = MixerparameterDefinition("Delay B", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_DELAY_B].SetMinMaxStandardDecimals_Float(FX_CHORUS_DELAY_B_MIN, FX_CHORUS_DELAY_B_MAX, FX_CHORUS_DELAY_B_DEFAULT, 0);
    // chorus phase
    mpm[MIXERPARAMETER_FX_CHORUS_PHASE_A] = MixerparameterDefinition("Phase A", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_PHASE_A].SetMinMaxStandardDecimals_Float(FX_CHORUS_PHASE_A_MIN, FX_CHORUS_PHASE_A_MAX, FX_CHORUS_PHASE_A_DEFAULT, 0);
    mpm[MIXERPARAMETER_FX_CHORUS_PHASE_B] = MixerparameterDefinition("Phase B", MIXERPARAMETER_UOM_NONE, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_PHASE_B].SetMinMaxStandardDecimals_Float(FX_CHORUS_PHASE_B_MIN, FX_CHORUS_PHASE_B_MAX, FX_CHORUS_PHASE_B_DEFAULT, 0);
    // chorus freq
    mpm[MIXERPARAMETER_FX_CHORUS_FREQ_A] = MixerparameterDefinition("Freq A", MIXERPARAMETER_UOM_KHZ, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_FREQ_A].SetMinMaxStandardDecimals_Float(FX_CHORUS_FREQ_A_MIN, FX_CHORUS_FREQ_A_MAX, FX_CHORUS_FREQ_A_DEFAULT, 0);
    mpm[MIXERPARAMETER_FX_CHORUS_FREQ_B] = MixerparameterDefinition("Freq B", MIXERPARAMETER_UOM_KHZ, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_FREQ_B].SetMinMaxStandardDecimals_Float(FX_CHORUS_FREQ_B_MIN, FX_CHORUS_FREQ_B_MAX, FX_CHORUS_FREQ_B_DEFAULT, 0);
    // chorus mix
    mpm[MIXERPARAMETER_FX_CHORUS_MIX] = MixerparameterDefinition("Mix", MIXERPARAMETER_UOM_PERCENT, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_CHORUS_MIX].SetMinMaxStandardDecimals_Float(FX_CHORUS_FREQ_B_MIN, FX_CHORUS_FREQ_B_MAX, FX_CHORUS_FREQ_B_DEFAULT, 0);

    // transientshaper

    // fast
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_FAST] = MixerparameterDefinition("Fast", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_FAST].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 1.0f, 0);
    // medium
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_MEDIUM] = MixerparameterDefinition("Medium", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_MEDIUM].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 15.0f, 0);
    // slow
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_SLOW] = MixerparameterDefinition("Slow", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_SLOW].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 150.0f, 0);
    // attack
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_ATTACK] = MixerparameterDefinition("Attack", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_ATTACK].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 3.0f, 0);
    // sustain
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_SUSTAIN] = MixerparameterDefinition("Sustain", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_SUSTAIN].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 1.0f, 0);
    // delay
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_DELAY] = MixerparameterDefinition("Delay", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_TRANSIENTSHAPER_DELAY].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 1.0f, 0);

    // FX_TYPE_OVERDRIVE            preGain Q hpfInputFreq lpfInputFreq lpfOutputFreq

    // delay A/B
    mpm[MIXERPARAMETER_FX_DELAY_DELAY_A] = MixerparameterDefinition("Delay A", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_DELAY_DELAY_A].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 350.0f, 0);
    mpm[MIXERPARAMETER_FX_DELAY_DELAY_B] = MixerparameterDefinition("Delay B", MIXERPARAMETER_UOM_MS, X32_MIXER_CHANGED_FX, X32_VCHANNEL_CHANGED_NONE, true);
    mpm[MIXERPARAMETER_FX_DELAY_DELAY_B].SetMinMaxStandardDecimals_Float(0.0f, 1000.0f, 450.0f, 0);

    // FX_TYPE_MULTIBANDCOMPRESOR   channel band threshold ratio attack  hold   release   makeup
    // FX_TYPE_DYNAMICEQ            band type freq staticGain  maxDynGain  Q  thresh  ratio  attack  release
}
