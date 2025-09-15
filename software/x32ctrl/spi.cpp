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

#include "x32ctrl.h"

// two defines for FPGA-configuration
#define PROG_B_GPIO_OFFSET 30
#define DONE_GPIO_OFFSET 2
#define FPGA_BUFFER_SIZE 4096
#define FPGA_FILE_BUFFER_SIZE 1024

int spiDspHandle[2];

// configures a Xilinx Spartan 3A via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
int spiConfigureFpga(const char* bitstream_path) {
    int spi_fd = -1;
    FILE *bitstream_file = NULL;
    int ret = 0;
    struct spi_ioc_transfer tr = {0};
    uint8_t* tx_buffer = (uint8_t*)malloc(FPGA_BUFFER_SIZE); // use heap instead of stack
    uint8_t* rx_buffer = (uint8_t*)malloc(FPGA_BUFFER_SIZE); // use heap instead of stack
    uint8_t* file_buffer = (uint8_t*)malloc(FPGA_FILE_BUFFER_SIZE); // use heap instead of stack
    uint16_t offset = 0;
    size_t bytes_read;

    uint8_t spiMode = SPI_MODE_0; // Xilinx Spartan-3A uses MODE 0
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    x32debug("Connecting to SPI for FPGA...\n");
    spi_fd = open(SPI_DEVICE_FPGA, O_RDWR);
    if (spi_fd < 0) {
        x32log("Error: Could not open SPI-device\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0; Xilinx FPGAs oft Modus 0 oder 2)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);

    // read bitstream-file and search for RAW-data
    long file_size = getFileSize(bitstream_path);
    if (file_size <= 0) {
        x32log("Error: Problem with bitstream-file\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        x32log("Error: Could not open bitstream-file\n");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }
    if ((bytes_read = fread(file_buffer, 1, FPGA_FILE_BUFFER_SIZE, bitstream_file)) > 0) {
        if (file_buffer[0] != 0xFF) {
            // skip header
            offset = ((file_buffer[0]<<8) | file_buffer[1]) + 4;

            // find 'e' record in header
            while (file_buffer[offset] != 0x65) {
                // skip the record
                offset += (file_buffer[offset+1]<<8 | file_buffer[offset+2]) + 3;

                // exit if the next record isn't within the buffer
                if (offset >= 1022) {
                    x32log("Error: Cannot find expected header in bit-file!\n");
                    if (bitstream_file) fclose(bitstream_file);
                    if (spi_fd >= 0) close(spi_fd);
                    free(tx_buffer);
                    free(rx_buffer);
                    free(file_buffer);
                    return -1;
                }
            }

            // skip the field name and bitstrem length
            offset += 5;

            x32debug("Detected bit-file with header...\n");
        }else{
                x32debug("Detected bin-file without header...\n");
                offset = 0; // start reading at byte 0
        }
    }
    fclose(bitstream_file);
    // update file_size to match payload-size
    file_size -= offset;

    // now open the file again and jump of the header (if any)
    x32debug("Configuring Xilinx Spartan-3A...\n");
    bitstream_file = fopen(bitstream_path, "rb");
    if (!bitstream_file) {
        x32log("Error: Could not open bitstream-file\n");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }
    // read number of header-bytes to buffer (will remain unused and will be overwritten)
    if (offset > 0) {
        fread(tx_buffer, 1, offset, bitstream_file);
    }

    // now send the data
    x32debug("Sending bitstream to FPGA...\n");
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    x32debug("Setting PROG_B-Sequence HIGH -> LOW -> HIGH and start upload...\n");
    int fdResetFpga = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
    write(fdResetFpga, "1", 1);
    usleep(500); // assert PROG_B at least 500ns (here 500us) to restart configuration process (see page 56 of UG332 v1.7)
    write(fdResetFpga, "0", 1);
    close(fdResetFpga);
    usleep(500);

    // send data
    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    while ((bytes_read = fread(tx_buffer, 1, FPGA_BUFFER_SIZE, bitstream_file)) > 0) {
        tr.len = bytes_read;
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            x32log("Error: SPI-transmission failed\n");
            if (bitstream_file) fclose(bitstream_file);
            if (spi_fd >= 0) close(spi_fd);
            free(tx_buffer);
            free(rx_buffer);
            free(file_buffer);
            return -1;
        }

        // calculate progress-bar
        total_bytes_sent += bytes_read;
        int progress = (int)((double)total_bytes_sent / file_size * progress_bar_width);
        printf("\rFPGA [");
        for (int i = 0; i < progress_bar_width; ++i) {
            if (i < progress) {
                printf("█");
            }else{
                printf(" ");
            }
        }
        printf("] %ld/%ld Bytes (%.2f%%)", total_bytes_sent, file_size, (double)total_bytes_sent / file_size * 100);
        fflush(stdout);
    }
    printf("\n");

/*
    // write additional dummy-data (ones = 0xFF = 0b11111111) to finalize configuration
    tr.len = 8; // eight additional bytes
    for (uint16_t i=0; i<100; i++) {
        tx_buffer[i] = 0xFF;
        rx_buffer[i] = 0;
    }
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
*/
    fclose(bitstream_file);
    close(spi_fd);
    free(tx_buffer);
    free(rx_buffer);
    free(file_buffer);
    usleep(50000); // wait 50ms

    return 0;
}


// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
// callit like: spiConfigureDsp(argv[1], argv[2], 2)
//
// /RESET: 1 -> 0 -> 1 (Linux already inverts the reset for us)
// wait at least 256 us
// Load data:
//   - load Kernel: 1536 8-bit words (equals 256 48-bit Words)
//   - load application (user dependent)
//   - load IVT (1536 8-bit words)
// DMA-Transfer expects a seemless data-transport while the manual says something about handshake and wait-states... strange
//
int spiConfigureDsp(const char* bitstream_path_a, const char* bitstream_path_b, uint8_t numStreams) {
    if ((numStreams < 1) || (numStreams > 2)) {
        return -1;
    }

    int spi_fd[2] = {-1};
    FILE *bitstream_file[2] = {NULL};
    int ret = 0;
    struct spi_ioc_transfer tr = {0};
    size_t bytesRead;
    uint8_t spiTxData[32];
    uint8_t spiRxData[sizeof(spiTxData)];
    long file_size[2];
    long totalBytesSent = 0;
    int progress_bar_width = 50;

    uint8_t spiMode = SPI_MODE_3; // AnalogDevices uses MODE 3 (CPOL=1, CPHA=1)
    uint8_t spiBitsPerWord = 32;
    uint32_t spiSpeed = SPI_SPEED_HZ;
//    uint8_t spiLsbFirst = 0; // Linux-driver for i.MX25 seems to have problems with this option

    // read size of bitstream-files
    file_size[0] = getFileSize(bitstream_path_a);
    if (file_size[0] <= 0) {
        x32log("Error: Problem with bitstream-file\n");
              return -1;
     }
    if (numStreams == 2) {
        file_size[1] = getFileSize(bitstream_path_b);
        if (file_size[1] <= 0) {
            x32log("Error: Problem with bitstream-file\n");
                  return -1;
         }
    }

    x32debug("Connecting to SPI for DSP1...\n");
    spi_fd[0] = open(SPI_DEVICE_DSP1, O_RDWR);
    if (spi_fd[0] < 0) {
        x32log("Error: Could not open SPI-device for DSP1\n");
        return -1;
    }
    if (numStreams == 2) {
        x32debug("Connecting to SPI for DSP2...\n");
        spi_fd[1] = open(SPI_DEVICE_DSP2, O_RDWR);
        if (spi_fd[1] < 0) {
            x32log("Error: Could not open SPI-device for DSP2\n");
            return -1;
        }
    }
    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd[0], SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd[0], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
//    ioctl(spi_fd[0], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst); // this seems to be ignored by i.MX25 linux-driver
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spi_fd[1], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spi_fd[1], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
//        ioctl(spi_fd[1], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst); // this seems to be ignored by i.MX25 linux-driver
    }

    // setup SPI-buffer
    tr.tx_buf = (unsigned long)spiTxData;
    tr.rx_buf = (unsigned long)spiRxData;
    tr.bits_per_word = spiBitsPerWord;
    tr.speed_hz = spiSpeed;

    // resetting DSPs
    int fdResetDsp = open("/sys/class/leds/reset_dsp/brightness", O_WRONLY);
    write(fdResetDsp, "1", 1); // assert reset of both DSPs
    usleep(800);

    // write some dummy-data to initialize SPI of i.MX25
    tr.len = 4; // send only 4 bytes
    ioctl(spi_fd[0], SPI_IOC_MESSAGE(1), &tr);
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_MESSAGE(1), &tr);
    }
    usleep(400);

    // release reset
    write(fdResetDsp, "0", 1);
    close(fdResetDsp);
    // wait at least 4096 16MHz clock-cycles (256 us) (see page 16-10 of ADSP-2137x SHARC Processor Hardware Reference v2.2)
    usleep(800);

    for (uint8_t i = 0; i < numStreams; i++) {
        bitstream_file[i] = fopen(bitstream_path_a, "rb");
        if (!bitstream_file[i]) {
            x32log("Error: Could not open bitstream-file\n");
            for (uint8_t i_b = 0; i_b < numStreams; i_b++) {
                if (bitstream_file[i_b]) fclose(bitstream_file[i_b]);
                if (spi_fd[i_b] >= 0) close(spi_fd[i_b]);
            }
            return -1;
        }

        // now send the data
        x32debug("Sending bitstream to DSP...\n");
        totalBytesSent = 0;
        progress_bar_width = 50;
        while ((bytesRead = fread(&spiTxData[0], 1, sizeof(spiTxData), bitstream_file[i])) > 0) {
            tr.len = bytesRead;

            // reverse bitorder as linux-driver seems to have no support for this
            reverseBitOrderArray(&spiTxData[0], bytesRead);

            ret = ioctl(spi_fd[i], SPI_IOC_MESSAGE(1), &tr);
            if (ret < 0) {
                x32log("Error: SPI-transmission failed\n");
                for (uint8_t i_b = 0; i_b < numStreams; i_b++) {
                    if (bitstream_file[i_b]) fclose(bitstream_file[i_b]);
                    if (spi_fd[i_b] >= 0) close(spi_fd[i_b]);
                }
                return ret;
            }

/*
            // check handshake
            if ((spiRxData[0] != 0xFF) && (spiRxData[1] != 0xFF) && (spiRxData[2] != 0xFF) && (spiRxData[3] != 0xFF)) {
                // wait for DSP
                while(1) {
                    ioctl(fd, SPI_IOC_MESSAGE(1), &tr); // read data from DSP
                    if ((spiRxData[0] == 0xFF) && (spiRxData[1] == 0xFF) && (spiRxData[2] == 0xFF) && (spiRxData[3] == 0xFF)) {
                        break; // DSP is ready for new data
                    }
                    usleep(100);
                }
            }
*/
            // calculate progress-bar
            totalBytesSent += bytesRead;
            int progress = (int)((double)totalBytesSent / file_size[i] * progress_bar_width);
            printf("\rDSP1 [");
            for (int i_progress = 0; i_progress < progress_bar_width; ++i_progress) {
                if (i_progress < progress) {
                    printf("█");
                }else{
                    printf(" ");
                }
            }
            printf("] %ld/%ld Bytes (%.2f%%)", totalBytesSent, file_size[i], (double)totalBytesSent / file_size[i] * 100);
            fflush(stdout);
        }
        printf("\n");

        fclose(bitstream_file[i]);
        close(spi_fd[i]);
        usleep(50000); // wait 50ms
    }

    return 0;
}

bool spiOpenDspConnections() {
    uint8_t spiMode = SPI_MODE_0; // user-program uses SPI MODE 0
    uint8_t spiBitsPerWord = 32;
    uint32_t spiSpeed = SPI_SPEED_HZ;

    for (uint8_t i = 0; i < 2; i++) {
        if (i == 0) {
            spiDspHandle[i] = open(SPI_DEVICE_DSP1, O_RDWR);
        }else if (i == 1) {
            spiDspHandle[i] = open(SPI_DEVICE_DSP2, O_RDWR);
        }
        if (spiDspHandle[i] < 0) {
            x32debug("Error: Could not open SPI-device");
            return false;
        }

        // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
        ioctl(spiDspHandle[i], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spiDspHandle[i], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spiDspHandle[i], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    }
    return true;
}

bool spiCloseDspConnections() {
    for (uint8_t i = 0; i < 2; i++) {
        if (spiDspHandle[i] >= 0) close(spiDspHandle[i]);
    }
    return true;
}

bool spiSendDspParameterArray(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]) {
    struct spi_ioc_transfer tr = {0};
//    uint32_t* spiTxData = (uint32_t*)malloc((valueCount + 3) * sizeof(uint32_t));
//    uint8_t* spiTxDataRaw = (uint8_t*)malloc((valueCount + 3) * sizeof(uint32_t));
//    uint8_t* spiRxDataRaw = (uint8_t*)malloc((valueCount + 3) * sizeof(uint32_t));
    uint32_t spiTxData[valueCount + 3]; // '*' + parameter + values + '#'
    uint8_t spiTxDataRaw[(valueCount + 3) * sizeof(uint32_t)];
    uint8_t spiRxDataRaw[sizeof(spiTxDataRaw)];
    int32_t bytesRead;

    tr.tx_buf = (unsigned long)spiTxDataRaw;
    tr.rx_buf = (unsigned long)spiRxDataRaw;
    tr.bits_per_word = 32;
    tr.speed_hz = SPI_SPEED_HZ;
    tr.len = sizeof(spiTxDataRaw);

    // send a command via SPI to DSP
    uint32_t parameter = ((uint32_t)valueCount << 24) + ((uint32_t)index << 16) + ((uint32_t)channel << 8) + (uint32_t)classId;

    spiTxData[0] = 0x0000002A; // StartMarker = '*'
    spiTxData[1] = parameter;
    memcpy(&spiTxData[2], &values[0], valueCount * sizeof(uint32_t));
    spiTxData[(valueCount + 3) - 1] = 0x00000023; // EndMarker = '#'
    memcpy(&spiTxDataRaw[0], &spiTxData[0], sizeof(spiTxDataRaw));

    bytesRead = ioctl(spiDspHandle[dsp], SPI_IOC_MESSAGE(1), &tr); // send via SPI

    //memcpy(&spiRxData[0], &spiRxDataRaw[0], 16); // copy received data to uint32_t-array
    // do something with data in spiRxData[4]

    //free(spiTxData);
    //free(spiTxDataRaw);
    //free(spiRxDataRaw);

    return (bytesRead > 0);
}

bool spiSendDspParameter(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, float value) {
    return spiSendDspParameterArray(dsp, classId, channel, index, 1, &value);
}

bool spiSendDspParameter_uint32(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint32_t value) {
    float value_f;
    memcpy(&value_f, &value, 4);
    return spiSendDspParameter(dsp, classId, channel, index, value_f);
}
