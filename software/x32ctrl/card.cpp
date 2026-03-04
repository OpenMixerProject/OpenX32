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

void Card::ProcessCommand(String command) {
	uint percentage;

    if (command.indexOf("*9N0") > -1){
        // command is send when a new card on slot 1 is detected
        // *9N0003B68500000024EB# -> empty 32GB card
        // *9N0003B2348000002459# -> 32GB card with four files
        // *9N0003A6D300# -> only information about the remaining space

        XLIVE_CardPresent[0] = (command.substring(4, 4).toInt() == 0);

        if (XLIVE_CardPresent[0]) {
            XLIVE_CardRemaingSpaceMB[0] = helper->hexToInt(command.substring(5, 5+8)) / 2000;
            if (command.length() == 22) {
                // we have information about the used space as well
                XLIVE_CardUsedSpaceMB[0] = helper->hexToInt(command.substring(13, 13+8)) / 2000;
            }
        }else{
            // no card installed
            XLIVE_CardRemaingSpaceMB[0] = 0;
            XLIVE_CardUsedSpaceMB[0] = 0;
        }

        // estimate total size of card
        XLIVE_CardTotalSpaceMB[0] = XLIVE_CardRemaingSpaceMB[0] + XLIVE_CardUsedSpaceMB[0];

        // update card-page if currently shown
        config->Refresh(CARD_STATE);
    }else if (command.indexOf("*9N1") > -1){
        // command is send when a new card on slot 2 is detected
        XLIVE_CardPresent[1] = (command.substring(4, 4).toInt() == 0);
        if (XLIVE_CardPresent[1]) {
            XLIVE_CardRemaingSpaceMB[1] = helper->hexToInt(command.substring(5, 5+8)) / 2000;
            if (command.length() == 22) {
                // we have information about the used space as well
                XLIVE_CardUsedSpaceMB[1] = helper->hexToInt(command.substring(13, 13+8)) / 2000;
            }
        }else{
            // no card installed
            XLIVE_CardRemaingSpaceMB[1] = 0;
            XLIVE_CardUsedSpaceMB[1] = 0;
        }

        // estimate total size of card
        XLIVE_CardTotalSpaceMB[1] = XLIVE_CardRemaingSpaceMB[1] + XLIVE_CardUsedSpaceMB[1];

        // update card-page if currently shown
        config->Refresh(CARD_STATE);
    }else if (command.indexOf("*9N22") > -1) {
        // we received current sample-position from expansion-card
        // *9N22xxxxxxxx#
        currentSongPositionSeconds = XLIVE_SampleIndexToSeconds(command.substring(5, command.length()-1));
        config->Refresh(CARD_POSITION);
    } else if (command.indexOf("*9N24") > -1) {
        // command after stop of recording
    }else if (command.indexOf("*9N07") > -1){
        // command after formatting the card
        // *9N0700000000#

    }
}

void Card::FlushRxBuffer() {
    adda->FlushRxBuffer();
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
        // *#
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

void Card::XLIVE_ReadRemainingCardSpace(uint card) {
	String ans = SendCommand("*9N" + String(card) + "#");
    //                              cardMissing
	// *9N" + intToHex(cardNumber, 1) + "0" + intToHex((cardSize-usedSpace)*2, 8) + intToHex(usedSpace*2, 8) + "#"
	// *9N00xxxxxxxxyyyyyyyy#
    // *9N0003B68500000024EB# -> empty 32GB card -> 62326272 MB total, 0 MB used
    // *9N0003B2348000002459# -> 32GB card with four files

	//uint remaining = helper->hexToInt(ans.substring(5, 5+8)) / 2;
	//uint used = helper->hexToInt(ans.substring(13, 13+8)) / 2;

    XLIVE_CardPresent[card] = (ans.substring(4, 4).toInt() == 0);

    if (XLIVE_CardPresent[card]) {
        XLIVE_CardRemaingSpaceMB[card] = helper->hexToInt(ans.substring(5, 5+8)) / 2000;
        if (ans.length() == 22) {
            // we have information about the used space as well
            XLIVE_CardUsedSpaceMB[card] = helper->hexToInt(ans.substring(13, 13+8)) / 2000;
        }
    }else{
        // no card installed
        XLIVE_CardRemaingSpaceMB[card] = 0;
        XLIVE_CardUsedSpaceMB[card] = 0;
    }
}

void Card::XLIVE_ReadTotalCardSpaceMB(uint card) {
	String ans = SendCommand("*9G" + String(card) + "#");
    //                          cardMissing
	// "*9G" + String(cardNumber) + "0" + intToHex(cardSize*2, 8) + "#"
	// *9G00xxxxxxxx#
    // *9G0003B70600# for a 32GB card -> 62326272 / (2*32 / 1000000) = 

	XLIVE_CardTotalSpaceMB[card] = helper->hexToInt(ans.substring(5, 5+8)) / 2000;
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

    String channelcount;
    switch (config->GetUint(CARD_NUMBER_OF_CHANNELS)) {
        case 0:
            channelcount = "32";
            break;
        case 1:
            channelcount = "16";
            break;
        case 2:
            channelcount = "08";
            break;
        case 3:
            channelcount = "32";
            break;
        case 4:
            channelcount = "08";
            break;
        case 5:
            channelcount = "02";
            break;
    }

	return (String("*9Y00#") == SendCommand("*9H" + session + channelcount + "0" +  "#")); // TODO: check what the trailing zero before the "#" does
}

bool Card::XLIVE_FormatCard() {
	return (String("*9Y00#") == SendCommand("*9Q~#"));
}

bool Card::XLIVE_SelectCard(uint card) {
	return (String("*9R0" + String(card) + "#") == SendCommand("*9R" + String(card) + "#"));
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