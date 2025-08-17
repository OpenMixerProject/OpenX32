#include "adda.h"

char uartBufferAdda[256]; // buffer for UART-readings
int addaPacketBufLen = 0;
char addaPacketBuffer[ADDA_MAX_PACKET_LENGTH];

void addaInit(uint32_t samplerate) {
  // send identification-commands to all possible boards (not sure if this is correct for smaller X32)
  addaSendCmd("*0I#"); // ADDA-Board
  usleep(10000);
  addaSendCmd("*1I#"); // ADDA-Board
  usleep(10000);
  addaSendCmd("*2I#"); // ADDA-Board
  usleep(10000);
  addaSendCmd("*3I#"); // ADDA-Board
  usleep(10000);
  addaSendCmd("*4I#"); // ADDA-Board
  usleep(10000);
  addaSendCmd("*5I#");  // ADDA-Board
  usleep(10000);
  addaSendCmd("*8I#"); // Expansion-Card
  usleep(10000);
  addaSendCmd("*8R#"); // Expansion-Card
  usleep(10000);

  // -1=unknown, 0=X32, 1=X32Compact, 2=X32Producer, 3=X32Rack, 4=X32Core
  // set gains to -2dB and turnoff phantom-power
  addaSendCmd("*1G00000#*1G00000#*1G10000#*1G10000#*1G20000#*1G20000#*1G30000#*1G30000#*1G40000#*1G40000#*1G50000#*1G50000#*1G60000#*1G60000#*1G70000#*1G70000#");
  addaSendCmd("*3G00000#*3G00000#*3G10000#*3G10000#*3G20000#*3G20000#*3G30000#*3G30000#*3G40000#*3G40000#*3G50000#*3G50000#*3G60000#*3G60000#*3G70000#*3G70000#");
  addaSendCmd("*0G00000#*0G00000#*0G10000#*0G10000#*0G20000#*0G20000#*0G30000#*0G30000#*0G40000#*0G40000#*0G50000#*0G50000#*0G60000#*0G60000#*0G70000#*0G70000#");
  addaSendCmd("*2G00000#*2G00000#*2G10000#*2G10000#*2G20000#*2G20000#*2G30000#*2G30000#*2G40000#*2G40000#*2G50000#*2G50000#*2G60000#*2G60000#*2G70000#*2G70000#");
  usleep(50000);

  // initialize the 8ch-input-boards
  addaSendCmd("*0R:W018B#");
  usleep(20000);
  addaSendCmd("*1R:W018B#");
  usleep(20000);
  addaSendCmd("*2R:W018B#");
  usleep(20000);
  addaSendCmd("*3R:W018B#");
  usleep(20000);

  // initialize the 8ch-output-boards
  addaSendCmd("*4R:W0281C0:W0800:W0280#");
  usleep(20000);
  addaSendCmd("*5R:W0281C0:W0800:W0280#");
  usleep(20000);

  if (samplerate == 44100) {
    // set all boards to 48kHz
    addaSendCmd("*8C40U#");
    usleep(10000);
    addaSendCmd("*8R8#*8C40U#");
    usleep(10000);
    addaSendCmd("*8C40U#");
    usleep(10000);
  } else if (samplerate == 48000) {
    // set all boards to 48kHz
    addaSendCmd("*8C80U#");
    usleep(10000);
    addaSendCmd("*8R8#*8C80U#");
    usleep(10000);
    addaSendCmd("*8C80U#");
    usleep(10000);
  } else if (samplerate == 96000) {
    // currently unknown, but the X32 is able to handle 96kHz for sure
  } else {
    // unsupported sample-rate
  }

  // and send some more commands (no information about the usage up to now)
  addaSendCmd("*9R0#*9G0#");
  usleep(10000);
  addaSendCmd("*9AF#");
  usleep(10000);
  addaSendCmd("*9N0#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9N0#");
  usleep(10000);
}

void addaSetSamplerate(uint32_t samplerate) {
  if (samplerate == 44100) {
    addaSendCmd("*8R#*8C40U#");
    usleep(10000);
  } else if (samplerate == 48000) {
    addaSendCmd("*8R#*8C80U#");
    usleep(10000);
  } else if (samplerate == 96000) {
    // currently unknown, but the X32 is able to handle 96kHz for sure
  } else {
    // unsupported sample-rate
  }

  // send initialization-commands to all boards
  addaSendCmd("*9R0#");
  usleep(10000);
  addaSendCmd("*9AF#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9AN#");
  usleep(10000);
  addaSendCmd("*9N0#");
  usleep(10000);
}

void addaSetGain(uint8_t boardId, uint8_t channel, float gain, bool phantomPower) {
  x32debug("addaSetGain(%d, %d, %f, %d)\n", boardId, channel, gain, phantomPower);

  messageBuilderInit(&message);

  messageBuilderAddRawByte(&message, '*');
  messageBuilderAddRawByte(&message, boardId + 0x30); // convert to ASCII-number
  messageBuilderAddRawByte(&message, 'G');
  messageBuilderAddRawByte(&message, (channel-1) + 0x30); // convert to ASCII-number

  // convert gain-value between -2dB and +45.5dB to three-digit value
  char buf[4];
  uint8_t gainHardware = (uint8_t)((gain + 2.0f)/2.5f); // gain is between -2dB and +45.5dB in 2.5dB steps
  sprintf(buf, "%03d", gainHardware);
  //valueToBinaryString(gain, &buf);
  messageBuilderAddString(&message, &buf[0]);

  if (phantomPower) {
    messageBuilderAddRawByte(&message, '1');
  }else{
    messageBuilderAddRawByte(&message, '0');
  }
  messageBuilderAddRawByte(&message, '#');

  uartTx(&fdAdda, &message, false);
}

void addaSendCmd(char *cmd) {
  messageBuilderInit(&message);

  messageBuilderAddString(&message, cmd);

  uartTx(&fdAdda, &message, false);
};

void addaProcessUartData(int bytesToProcess) {
  uint8_t currentByte;

  if (bytesToProcess <= 0) {
    return;
  }

  for (int i = 0; i < bytesToProcess; i++) {
    currentByte = (uint8_t)uartBufferAdda[i];
    //printf("%02X ", currentByte); // empfangene Bytes als HEX-Wert ausgeben

    // add received byte to buffer
    if (addaPacketBufLen < ADDA_MAX_PACKET_LENGTH) {
      addaPacketBuffer[addaPacketBufLen++] = currentByte;
    } else {
      // buffer full -> remove oldest byte
      memmove(addaPacketBuffer, addaPacketBuffer + 1, ADDA_MAX_PACKET_LENGTH - 1);
      addaPacketBuffer[ADDA_MAX_PACKET_LENGTH - 1] = currentByte;
    }

    int packetBegin = -1;
    int packetEnd = -1;
    int receivedPacketLength = 0; // length of detected packet

    // check if we received enought data to process at least the shortest message (e.g. *2Y#)
    if (addaPacketBufLen >= 3) {
      // check if received character is end of message ('#')
      if (addaPacketBuffer[addaPacketBufLen - 1] == '#') {
        // we received possible end of a message
        packetEnd = addaPacketBufLen - 1;

        // now search begin of message ('*')
        for (uint16_t j = 0; j < ADDA_MAX_PACKET_LENGTH; j++) {
          if (addaPacketBuffer[ADDA_MAX_PACKET_LENGTH - 1 - j] == '*') {
            // found begin of message
            packetBegin = ADDA_MAX_PACKET_LENGTH - 1 - j;
            break;
          }
        }

        receivedPacketLength = (packetEnd - packetBegin + 1);
        if ((packetBegin >= 0) && (packetEnd > 0) && (receivedPacketLength > 0)) {
          // we found a valid answer from the ADDA-boards

          // copy the message including * and # into a new buffer and 0-terminate it
          char *payload = malloc(sizeof(char) * (receivedPacketLength + 1));
          memcpy(payload, &addaPacketBuffer[packetBegin], receivedPacketLength);
          payload[receivedPacketLength] = '\0';

          // call the callback to do something with the received message
          addaCallback(payload);
          free(payload);

          // shift remaining bytes by processed amount of data
          memmove(addaPacketBuffer, addaPacketBuffer + receivedPacketLength, addaPacketBufLen - receivedPacketLength);
          addaPacketBufLen -= receivedPacketLength;
        }
      }
    }
  }
}

void addaSetMute(bool muted) {
    int fd = open("/sys/class/leds/audio_mute/brightness", O_WRONLY);

    if (muted) {
        write(fd, "1", 1);
    } else {
        write(fd, "0", 1);
    }
    close(fd);
}

int8_t addaGetBoardId(uint8_t channel) {
    // we have up to 4 boards. Now we have to find the right BoardId for the desired channel

    // TODO: check why the order is 1->3->0->2 and if it could change between X32-types and even startups
    if ((channel >= 1) && (channel <= 8)) {
        return 1;
    }else if ((channel >= 9) && (channel <= 16)) {
        return 3;
    }else if ((channel >= 17) && (channel <= 24)) {
        return 0;
    }else if ((channel >= 25) && (channel <= 32)) {
        return 2;
    }else{
        return -1;
    }
}
