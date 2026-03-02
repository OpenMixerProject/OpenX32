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

#include "card.h"

Card::Card(X32BaseParameter* basepar, Adda* _adda) : X32Base(basepar) {
    this->adda = _adda;
}

void Card::Init() {
    // TODO - move from ADDA Class

    // detect type of card
    if (!adda->HasExpansion()) {
        type = CARD_TYPE_NONE;
        return;
    }

    String typeString = adda->GetExpansion();
    if ((typeString.indexOf("X-UF") > 0) || (typeString.indexOf("X-USB") > 0)) {
        type = CARD_TYPE_XUSB;
    } else if (typeString.indexOf("X-UREC") > 0) {
        type = CARD_TYPE_XLIVE;
    } else if (typeString.indexOf("X-ADAT") > 0) {
        type = CARD_TYPE_XADAT;
    } else if (typeString.indexOf("X-MADI") > 0) {
        type = CARD_TYPE_XMADI;
    } else {
        type = CARD_TYPE_UNKNOWN; // unknown card
    }
}

void Card::Sync()
{
    if (config->HasParameterChanged(CARD_NUMBER_OF_CHANNELS) || config->HasParameterChanged(CARD_AUDIO_SOURCE))
    {
        XUSB_XLIVE_SetConfig(config->GetUint(CARD_NUMBER_OF_CHANNELS), config->GetUint(CARD_AUDIO_SOURCE));
    }
}

String Card::SendCommand(String command){
	return adda->SendReceive(command);
}

void Card::XUSB_XLIVE_SetConfig(uint8_t channelparameter, uint source)
{
    if ((type != CARD_TYPE_XUSB) && (type != CARD_TYPE_XLIVE))
    {
        // only X-LIVE and X-USB support this command
        return;
    }

	// "X-USB:A:3"
	//
	// channelparameter - in / out
	// 0 - 32 / 32
	// 1 - 16 / 16
	// 2 - 32 / 8
	// 3 - 8 / 32
	// 4 - 8 / 8
	// 5 - 2 / 2

	String command = "*8C8" + String(channelparameter);

    if (type == CARD_TYPE_XLIVE) {
        // for X-LIVE we need to set the source as well (SD-Card or USB-Interface)
        // only X-Live
        if (source == 0) {
            command += "U"; // USB-Interface
        } else {
            command += "C"; // SD-Card
        }
    }
	
	command += "#";

	SendCommand(command);
}

bool Card::XLIVE_Stop()
{
	SendCommand("*9F#");
    XLIVE_Recording = false;
    XLIVE_Playing = false;

	/*
	Card answers with a bunch of results if recording is stopped
	*9N24003F38D0# -> probably info-command for new entries?

	*9N0003A6DD80#
	*9N0003A6D300#

	*9N24003F47D0#

	*9N0003A6D300#
	*9N24003F50A0#

	*9N24003F56D0#
	*/

	return true;
}

bool Card::XLIVE_PlayPause() {
    String ans;
    if (XLIVE_Playing) {
        ans = SendCommand("*9E#");
        XLIVE_Playing = false;
        return (ans == String("*9E00#"));
    } else {
        ans = SendCommand("*9D#");
        XLIVE_Playing = true;
        return (ans == String("*9D00#"));
    }
}

bool Card::XLIVE_Seek(uint sampleIndex) {
	//*9M0009EDC0#Q#
	String indexHex = "00000000";
	return (SendCommand("*9M" + indexHex + "#") == String("*9M00#"));
}

String Card::XLIVE_RequestToc(uint* numberOfEntries) {
	// request entries
	String TOC;
	String newEntry;
    *numberOfEntries = 0;

    // Request first entry of TOC
	newEntry = SendCommand("*9AF#");
    if (newEntry.indexOf("AEN00") > 0) {
        // no entries here
        return "";
    }

    if (newEntry.indexOf("ASF") > 0) {
        // received first entry
        TOC += newEntry.substring(5, 5+8) + ",";
        (*numberOfEntries)++;
    }

	for (uint i = 0; i < 100; i++) {
		newEntry = SendCommand("*9AN#");

        // check if we reached end of TOC
		if (newEntry == String("*9AEN00#")) {
			// end of TOC
			break;
		}else if (newEntry.indexOf("ASN") > 0){
			// received entry in the form *9ASN________#
			TOC += newEntry.substring(5, 5+8) + ",";
            (*numberOfEntries)++;
		}else{
            // unexpected answer, stop requesting more entries to avoid infinite loop
            break;
        }
	}

	return TOC;
}

uint Card::XLIVE_Remaining() {
	String ans = SendCommand("*9N");

	// *9N" + intToHex(cardNumber, 1) + "0" + intToHex((cardSize-usedSpace)*2, 8) + intToHex(usedSpace*2, 8) + "#"
	// *9N00xxxxxxxxyyyyyyyy#

	uint remaining = helper->hexToInt(ans.substring(5, 5+8));
	uint used = helper->hexToInt(ans.substring(13, 13+8));

	return remaining;
}

uint Card::XLIVE_Total() {
	String ans = SendCommand("*9G");

	// "*9G" + String(cardNumber) + "0" + intToHex(cardSize*2, 8) + "#"
	// *9G00xxxxxxxx#

	uint total = helper->hexToInt(ans.substring(5, 5+8));
	
	return total;
}

bool Card::XLIVE_SelectInterface(uint option, uint interface) {
	// command is *8C8od#
	// o = options between 0 ... 5
	// d = interface 0 = USB, 1 = CARD

	return (String("*8Y#") == SendCommand("*8C8" + String(option) + String(interface) + "#"));
}

bool Card::XLIVE_DeleteSession(String session) {
	return (String("*9Y00#") == SendCommand("*9X" + session + "#"));
}

bool Card::XLIVE_RecordNewSession() {
    // first calculate new session name based on current date and time
    std::time_t jetzt = std::time(nullptr);
    std::tm* tm = std::localtime(&jetzt);
	String session = XLIVE_DateTimeToSessionName(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);

    // now start recording with this session name
    XLIVE_Recording = true;
	return (String("*9Y00#") == SendCommand("*9H" + session +  "#"));
}

bool Card::XLIVE_FormatCard() {
	return (String("*9Y00#") == SendCommand("*9Q~#"));
}

bool Card::XLIVE_SelectCard(uint card) {
	return (String("*9R00#") == SendCommand("*9R" + String(card) + "#"));
}

bool Card::XLIVE_SelectSession(String session) {
    String ans = SendCommand("*9B" + session +  "#"); // maybe add #Q at this command?

    // ans contain the following information:
    // *9B000 00             32        0     0BD1A300 20ED80900# // -> zero markers
    // *9B000 02             32        0     00F4E900 000000000# // two markers
    // *9B000 MARKERCOUNT CHANNELCOUNT 0 NUMBEROFTOTALSAMPLES 0 ???

    //uint currentSongMarkerCount = ans.substring(6, 6+2).toInt();
    currentSongNumberChannels = ans.substring(8, 8+2).toInt();
    currentSongTotalSeconds = (float)helper->hexToInt(ans.substring(11, 11+8)) / (float)config->GetUint(SAMPLERATE);

    return true;
}

String Card::XLIVE_SecondsToSampleIndex(uint seconds) {
    // X-Live works with 48kHz (or 44.1kHz) sample rate, so we can calculate the sample index from the time in seconds
    uint32_t sampleIndex = seconds * (float)config->GetUint(SAMPLERATE);

    return helper->intToHex(sampleIndex, 8);
}

uint Card::XLIVE_SampleIndexToSeconds(String sampleIndexHex) {
    uint32_t sampleIndex = helper->hexToInt(sampleIndexHex);

    // X-Live works with 48kHz (or 44.1kHz) sample rate, so we can calculate the time from the sample index
    uint32_t seconds = sampleIndex / (float)config->GetUint(SAMPLERATE);

    return seconds;
}

String Card::XLIVE_SessionNameToString(String timecodeHex) {
  uint32_t timecode = helper->hexToInt(timecodeHex);

  uint16_t year = (timecode >> 25) + 1980;
  uint8_t month =  (timecode & 0x1FFFFFF) >> 21;
  uint8_t day = (timecode & 0x1FFFFF) >> 16;
  uint8_t hour = (timecode & 0xFFFF) >> 11;
  uint8_t minute =  (timecode & 0x7FF) >> 5;
  uint8_t second =  (timecode & 0x1F) << 1;

  return helper->intToStringTwoDigits(day) + "." + helper->intToStringTwoDigits(month) + "." + String(year) + " " + helper->intToStringTwoDigits(hour) + ":" + helper->intToStringTwoDigits(minute) + ":" + helper->intToStringTwoDigits(second);
}

String Card::XLIVE_DateTimeToSessionName(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second) {
  uint32_t timecode = (uint32_t)((year - 1980)) << 25;
  timecode += (uint32_t)month << 21;
  timecode += (uint32_t)day << 16;
  timecode += (uint32_t)hour << 11;
  timecode += (uint32_t)minute << 5;
  timecode += (uint32_t)second << 1;

  return helper->intToHex(timecode, 8);
}