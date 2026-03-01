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

#include "spi.h"

SPI::SPI(X32BaseParameter* basepar) : X32Base(basepar) {}

// configures a Xilinx Spartan 3A via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
// Xilinx FPGAs do *not* use ChipSelect during SPI Slave Programming!
int SPI::UploadBitstreamFpgaXilinx(void) {

    string filename_xilinx = app->get_option("--X")->as<string>();

    // abort if no file path was given
    if(filename_xilinx.length() == 0)
    {
        return -1;
    }

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
    uint32_t spiSpeed = 2 * state->fpga_spi_speed; // Linux SPI driver seems to divide speed by 2

    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Connecting to SPI for FPGA...");
    if (state->raspi)
    {
        spi_fd = open("/dev/spidev0.0", O_RDWR);
    }
    else
    {
        spi_fd = open(SPI_DEVICE_FPGA, O_RDWR);
    }

    if (spi_fd < 0)
    {
        helper->Error("Could not open SPI-device\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    // SPI-Modus (0 = CPOL=0, CPHA=0; Xilinx FPGAs oft Modus 0 oder 2)
    ioctl(spi_fd, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).", SPI_DEVICE_FPGA, spiMode, spiSpeed/2);

    // read bitstream-file and search for RAW-data
    long file_size = helper->GetFileSize(filename_xilinx.c_str());
    if (file_size <= 0) {
        helper->Error("Problem with bitstream-file\n");
        free(tx_buffer);
        free(rx_buffer);
        free(file_buffer);
        return -1;
    }

    bitstream_file = fopen(filename_xilinx.c_str(), "rb");
    if (!bitstream_file) {
        helper->Error("Could not open bitstream-file\n");
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
                    helper->Error("Cannot find expected header in bit-file!\n");
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

            helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Detected bit-file with header...");
        }else{
                helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Detected bin-file without header...");
                offset = 0; // start reading at byte 0
        }
    }
    fclose(bitstream_file);
    // update file_size to match payload-size
    file_size -= offset;

    // now open the file again and jump of the header (if any)
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Configuring Xilinx Spartan-3A...");
    bitstream_file = fopen(filename_xilinx.c_str(), "rb");
    if (!bitstream_file) {
        helper->Error("Could not open bitstream-file\n");
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
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending bitstream to FPGA...");
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.bits_per_word = spiBitsPerWord;
    tr.cs_change = 0;
    tr.delay_usecs = 0;

    toggleFpgaProgramnPin(500, 500); // start configuration by toggling PROGRAMN-pin

    // send data
    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    int last_progress = -1;
    int current_progress;
    while ((bytes_read = fread(tx_buffer, 1, FPGA_BUFFER_SIZE, bitstream_file)) > 0) {
        tr.len = bytes_read;
        ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            helper->Error("SPI-transmission failed\n");
            if (bitstream_file) fclose(bitstream_file);
            if (spi_fd >= 0) close(spi_fd);
            free(tx_buffer);
            free(rx_buffer);
            free(file_buffer);
            return -1;
        }

        // calculate progress-bar
        total_bytes_sent += bytes_read;
        current_progress = (total_bytes_sent * 100) / file_size;
        if ((current_progress > (last_progress + 5)) || (total_bytes_sent == file_size)) {
            int progress = (int)((float)total_bytes_sent / file_size * progress_bar_width);
            printf("\rFPGA [");  //TODO implement with helper->Log()
            for (int i = 0; i < progress_bar_width; ++i) {
                if (i < progress) {
                    printf("█");
                }else{
                    printf(" ");
                }
            }
            printf("] %ld/%ld Bytes (%u %%)", total_bytes_sent, file_size, (uint)((total_bytes_sent * 100) / file_size));
            fflush(stdout);

            last_progress = current_progress;
        }
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

// configures a Lattice ECP5 via SPI
// returns 0 if successul, -1 on errors
bool SPI::UploadBitstreamFpgaLattice(void) {

    string filename_lattice = app->get_option("--L")->as<string>();

    // abort if no file path was given
    if(filename_lattice.length() == 0)
    {
        return false;
    }

    int spi_fd = -1;
	uint32_t status;
    FILE *bitstream_file = NULL;

    uint8_t spiMode = SPI_MODE_3; // both SPI_MODE_0 and SPI_MODE_3 can be used as the Lattice ECP5-FPGA is reading on rising edge
    uint8_t spiBitsPerWord = 8;
    uint32_t spiSpeed = 2 * state->fpga_spi_speed; // Linux SPI driver seems to divide speed by 2

	setFpgaDonePin(false); // deassert DONE-pin
	
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Connecting to SPI...");
    spi_fd = open(SPI_DEVICE_FPGA, O_RDWR);
    if (spi_fd < 0) {
        perror("Error: Could not open SPI-device\n");
        return false;
    }

    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "SPI-Bus '%s' initialized. (Mode %d, Speed %d Hz).", SPI_DEVICE_FPGA, spiMode, spiSpeed/2);

    if (helper->DEBUG_SPI(DEBUGLEVEL_NORMAL)){
	    status = fpgaLatticeReadData(&spi_fd, CMD_LSC_READ_STATUS);
        fpgaLatticePrintBits(status);
    }

    // open the file

    struct stat st;
    size_t bitstream_size;
    if (stat(filename_lattice.c_str(), &st) == 0) {
        bitstream_size = st.st_size;
    }
    bitstream_file = fopen(filename_lattice.c_str(), "rb");
    if (!bitstream_file) {
        perror("Error: Could not open bitstream-file");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return false;
    }

    // perform configuration-process
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Configuring Lattice FPGA...");

	// first activate the configuration-mode by toggle the PROGRAMN-pin
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Setting PROGRAMN-Sequence HIGH -> LOW -> HIGH and start upload...");
	toggleFpgaProgramnPin(4000, 50000);

    // read IDCODE
    uint32_t idcode = fpgaLatticeReadData(&spi_fd, CMD_READ_ID);
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Read IDCODE: 0x%08X", idcode);
	// check if we've found the ID for the Lattice LFE5U-25 FPGA
	if (idcode != 0x41111043) {
		helper->Error("Error: Unexpected IDCODE: 0x%08X\n", idcode);
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return false;
	}

    // Enable SRAM Programming: send ISC_ENABLE command [class C command]
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending ISC_ENABLE...");
    fpgaLatticeSendCommand(&spi_fd, CMD_ISC_ENABLE, false, false);
    
	// =========== BEGIN of bitstream-transmitting without deasserting ChipSelect ===========

    // transmit large bitstream in chunks but without deasserting CS
    fseek(bitstream_file, 0, SEEK_SET); 
    const size_t CHUNK_SIZE = 1024; // 1 KB as maximum chunk-size
    uint8_t tx_chunk[CHUNK_SIZE];
    const int MAX_TRANSFERS = (int)(bitstream_size / CHUNK_SIZE) + 1;
	
	// dynamic allocation of memory for the transfer-array
    struct spi_ioc_transfer *transfers = (spi_ioc_transfer*)calloc(MAX_TRANSFERS, sizeof(struct spi_ioc_transfer));
    if (!transfers) {
        perror("Error: Failed to allocate memory for transfer structures");
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return false;
    }
    // buffer for data-payload
    uint8_t *bitstream_payload = (uint8_t *)malloc(bitstream_size);
    if (!bitstream_payload) {
        perror("Error: Failed to allocate memory for bitstream payload");
        free(transfers);
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return false;
    }
    // read buffer into large payload-buffer
    size_t total_bytes_read = fread(bitstream_payload, sizeof(char), bitstream_size, bitstream_file);
    if (total_bytes_read != bitstream_size) {
        fprintf(stderr, "Error: Failed to read complete bitstream file.\n");
        free(bitstream_payload);
        free(transfers);
        if (bitstream_file) fclose(bitstream_file);
        if (spi_fd >= 0) close(spi_fd);
        return false;
    }

    if (helper->DEBUG_SPI(DEBUGLEVEL_NORMAL)) {
        int p = 20;
        printf("\n\n-----------------------------------------\n");
        printf("first and last %d bytes of bitstream:\n\n", p);
    
        for (int i=0; i < p; i++){
            printf("0x%.2X ", bitstream_payload[i]);
        }
        printf("\n...");
        printf("\n...");
        printf("\n...\n");
        for (int i=p; i > 0; i--){
            printf("0x%.2X ", bitstream_payload[bitstream_size - i]);
        }
        printf("\n-----------------------------------------\n\n");
    }
    
    setFpgaChipSelectPin(true);

    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending LSC_BITSTREAM_BURST");
    fpgaLatticeTransferCommand(&spi_fd, CMD_LSC_BITSTREAM_BURST); // keep CS asserted after this command

    // configure transfer
    int num_transfers = 0;
    size_t current_offset = 0;
    long total_bytes_sent = 0;
    int progress_bar_width = 50;
    int last_progress = -1;
    int current_progress;
    while (current_offset < bitstream_size) {
        size_t len = bitstream_size - current_offset;
        if (len > CHUNK_SIZE) {
            len = CHUNK_SIZE;
        }

		// get a temporary struct for the current transfer-element for configuration
        struct spi_ioc_transfer tr;

        tr.tx_buf = (unsigned long)(&bitstream_payload[current_offset]);
        tr.rx_buf = (unsigned long)(&bitstream_payload[current_offset]); // Kein Rx erforderlich
        tr.len = len;
        tr.bits_per_word = spiBitsPerWord;
        tr.cs_change = 0; // keep CS asserted between individual chunks, but deassert after last chunk. Behavior tested on RaspberryPi 4.
        
        int ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 0) {
            helper->Error("SPI-transmission failed\n");
			free(bitstream_payload);
			free(transfers);
            if (spi_fd >= 0) close(spi_fd);
            return false;
        }

        // calculate progress-bar
        total_bytes_sent += len;
        current_progress = (total_bytes_sent * 100) / bitstream_size;
        if ((current_progress > (last_progress + 5)) || (total_bytes_sent == bitstream_size)) {
            int progress = (int)((float)total_bytes_sent / bitstream_size * progress_bar_width);
            printf("\rFPGA [");  //TODO implement with helper->Log()
            for (int i = 0; i < progress_bar_width; ++i) {
                if (i < progress) {
                    printf("█");
                }else{
                    printf(" ");
                }
            }
            printf("] %ld/%ld Bytes (%u %%)", total_bytes_sent, bitstream_size, (uint)((total_bytes_sent * 100) / bitstream_size));
            fflush(stdout);

            last_progress = current_progress;
        }

        current_offset += len;
        num_transfers++;
    }

    // freeing allocated dynamic memory
    free(bitstream_payload);
    free(transfers);

    printf("\n");
    fflush(stdout); // immediately write to console!

	// =========== END of bitstream-transmitting without deasserting ChipSelect ===========
  
    setFpgaChipSelectPin(false);

    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending CMD_LSC_READ_STATUS");
    status = fpgaLatticeReadData(&spi_fd, CMD_LSC_READ_STATUS);
    if(helper->DEBUG_FPGA(DEBUGLEVEL_NORMAL)) {
        fpgaLatticePrintBits(status);
    }

    // Exit Programming Mode: send ISC_DISABLE
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending CMD_ISC_DISABLE");
	fpgaLatticeSendCommand(&spi_fd, CMD_ISC_DISABLE, false, true);

    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "wait 100 ms");
	usleep(100000); // wait 100 ms

    // check Status-Bits
    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending CMD_LSC_READ_STATUS");
	status = fpgaLatticeReadData(&spi_fd, CMD_LSC_READ_STATUS);
    if(helper->DEBUG_SPI(DEBUGLEVEL_NORMAL)) {
        fpgaLatticePrintBits(status);
    }
    bool ret =
        ((status) & (1<<(8))) &&  // Bit 8 (DONE) must be 1
        !((status) & (1<<(26)));  // Bit 26 (EXECUTION ERROR) must be 0

	if (bitstream_file) fclose(bitstream_file);
	if (spi_fd >= 0) close(spi_fd);
    
	return ret;
}

void SPI::toggleFpgaProgramnPin(uint32_t assertTime, uint32_t waitTime) {

    // raspi GPIO25
    if (state->raspi)
    {
        // assert PROGRAMN-pin (this sets the real GPIO to LOW)
        system("pinctrl set 25 dl");

        // we have to keep at least 25ns. So keep it 4ms asserted
        usleep(assertTime);

        // deassert PROGRAMN-pin (this sets the real GPIO to HIGH)
        system("pinctrl set 25 dh");

        // wait 50ms until FPGA is ready
        helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "wait %d ms", waitTime/1000);
        usleep(waitTime);
    }
    else 
    {
        int fd = open("/sys/class/leds/reset_fpga/brightness", O_WRONLY);
        write(fd, "1", 1); // assert PROGRAMN-pin (this sets the real GPIO to LOW)
        helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "PROGRAMN-pin set to LOW for %d ms", assertTime/1000);

        // we have to keep at least 25ns. So keep it 4ms asserted
        usleep(assertTime);

        write(fd, "0", 1); // deassert PROGRAMN-pin (this sets the real GPIO to HIGH)
        helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "PROGRAMN-pin set to HIGH");
        close(fd);
        
        // wait 50ms until FPGA is ready
        helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "wait %d ms", waitTime/1000);
        usleep(waitTime);
    }
}

void SPI::setFpgaChipSelectPin(bool state)
{
    if (!this->state->raspi)
    {
        int fd = open("/sys/class/leds/cs_fpga/brightness", O_WRONLY);
        if (state) {
            write(fd, "1", 1); // assert ChipSelect (sets the real pin to LOW)
            helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "asserted ChipSelect");
        }else{
            write(fd, "0", 1); // deassert ChipSelect (sets the real pin to HIGH)
            helper->DEBUG_SPI(DEBUGLEVEL_VERBOSE, "deasserted ChipSelect");
        }
        close(fd);
    }   
}

void SPI::setFpgaDonePin(bool state) {
	int fd = open("/sys/class/leds/done_fpga/brightness", O_WRONLY);
	if (state) {
		write(fd, "1", 1); // assert FPGA_DONE (sets the real pin to HIGH)
	}else{
		write(fd, "0", 1); // deassert FPGA_DONE (sets the real pin to LOW)
	}
	close(fd);
}

// ===============================================
// Functions for Lattice FPGA
// ===============================================

int SPI::fpgaLatticeReadData(int* spi_fd, uint8_t cmd) {
	// prepare ioc-message
    uint8_t tx_buf[8] = {0};
    uint8_t rx_buf[8] = {0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 8, // 4 byte for command + 4 byte for read
        .bits_per_word = 8
    };
    tx_buf[0] = cmd; // command

	// transmit and read message (8 bytes in total)
	setFpgaChipSelectPin(true); // assert ChipSelect
    ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);    
	setFpgaChipSelectPin(false); // deassert ChipSelect

	// copy desired data to 32-bit word
    int rx_data;
    memcpy(&rx_data, &rx_buf[4], 4);
	rx_data = fpgaLatticeReverseByteOrder_uint32(rx_data); // reverse byte-order

	return rx_data;
}

uint32_t SPI::fpgaLatticeReverseByteOrder_uint32(uint32_t x) { 
  return 
      ((x >> 24) & 0x000000FFul) | 
      ((x >>  8) & 0x0000FF00ul) | 
      ((x <<  8) & 0x00FF0000ul) | 
      ((x << 24) & 0xFF000000ul); 
} 

#define SHOWSTATUSBIT(status, bit, desc) if (status & (1<<(bit))) { printf("| %2d | %-30s | %1s |\n", bit, desc, ((status) & (1<<(bit))) ? "X" : " "); };

// Assumes little endian
void SPI::fpgaLatticePrintBits(uint32_t status)
{
    int i;
    
    printf("\n");
    printf("+-----------------------------------------+\n");
    printf("|      Lattice FPGA Status Register       |\n");
    printf("+----+--------------------------------+---+\n");
    printf("|Bit |Description                     |Set|\n");
    printf("+----+--------------------------------+---+\n");
      
    SHOWSTATUSBIT(status, 0, "JTAG Transparent");
    SHOWSTATUSBIT(status, 1, "Config Target Selection [0]");
    SHOWSTATUSBIT(status, 2, "Config Target Selection [1]");
    SHOWSTATUSBIT(status, 3, "Config Target Selection [2]");
    SHOWSTATUSBIT(status, 4, "JTAG Active");
    SHOWSTATUSBIT(status, 5, "PWD Protection");
    SHOWSTATUSBIT(status, 6, "(Internal use)");
    SHOWSTATUSBIT(status, 7, "Decrypt Enable");
    SHOWSTATUSBIT(status, 8, "DONE");
    SHOWSTATUSBIT(status, 9, "ISC Enable");
    SHOWSTATUSBIT(status, 10, "Write Enable");
    SHOWSTATUSBIT(status, 11, "Read Enable");
    SHOWSTATUSBIT(status, 12, "Busy Flag");
    SHOWSTATUSBIT(status, 13, "Fail Flag");
    SHOWSTATUSBIT(status, 14, "FEA OTP");
    SHOWSTATUSBIT(status, 15, "Decrypt Only");
    SHOWSTATUSBIT(status, 16, "PWD Enable");
    SHOWSTATUSBIT(status, 17, "(Internal use)");
    SHOWSTATUSBIT(status, 18, "(Internal use)");
    SHOWSTATUSBIT(status, 19, "(Internal use)");
    SHOWSTATUSBIT(status, 20, "Encrypt Preamble");
    SHOWSTATUSBIT(status, 21, "Std Preamble");
    SHOWSTATUSBIT(status, 22, "SPIm Fail 1");
    SHOWSTATUSBIT(status, 23, "BSE Error Code [0]");
    SHOWSTATUSBIT(status, 24, "BSE Error Code [1]");
    SHOWSTATUSBIT(status, 25, "BSE Error Code [2]");
    SHOWSTATUSBIT(status, 26, "ID Error");
    SHOWSTATUSBIT(status, 27, "ID Error");
    SHOWSTATUSBIT(status, 28, "Invalid Command");
    SHOWSTATUSBIT(status, 29, "SED Error");
    SHOWSTATUSBIT(status, 30, "Bypass Mode");
    SHOWSTATUSBIT(status, 31, "Flow Through Mode");
    
    printf("+----+--------------------------------+---+\n");
}

bool SPI::fpgaLatticePollBusyFlag(int* spi_fd) {
	uint8_t busyState;
	int timeout = 0;
	
	do {
		busyState = fpgaLatticeReadData(spi_fd, CMD_LSC_CHECK_BUSY);
		
		timeout++;
		if (timeout == 100) { // wait maximum 100 ms
			return false;
		}
		
		usleep(1000); // wait 1 ms until next read of status
	} while(busyState != 0);
		
	return true;
}

bool SPI::fpgaLatticeSendCommand(int* spi_fd, uint8_t cmd, bool keepCS, bool checkBusyAndStatus) {
	// prepare ioc-message
    uint8_t tx_buf[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t rx_buf[4] = {0x0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4, // Standard 4-Byte-Befehl (8-Bit Cmd + 24-Bit Dummy)
        .bits_per_word = 8
    };
	if (keepCS) {
		tr_cmd.cs_change = 1; // inverted logic here: 1 = no change=keep CS asserted, 0 = deassert CS after command
	}else{
		tr_cmd.cs_change = 0; // inverted logic here: 1 = no change=keep CS asserted, 0 = deassert CS after command
	}
    tx_buf[0] = cmd; // command

	// transmit message (4 bytes)
	setFpgaChipSelectPin(true); // assert ChipSelect
    int ret = ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);
	if(!keepCS){
		setFpgaChipSelectPin(false); // deassert ChipSelect
	}
	
	// optional: wait until busy-flag is reset
	if (checkBusyAndStatus) {
		if (!fpgaLatticePollBusyFlag(spi_fd)) {
			return false;
		}
        return true;
	}else{
		return (ret >= 0);
	}
};

int SPI::fpgaLatticeTransferCommand(int* spi_fd, uint8_t cmd) {
	// prepare ioc-message
    uint8_t tx_buf[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t rx_buf[4] = {0x0};
    struct spi_ioc_transfer tr_cmd = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4, // Standard 4-Byte-Befehl (8-Bit Cmd + 24-Bit Dummy)
        .bits_per_word = 8
    };
    tx_buf[0] = cmd; // command

	return ioctl(*spi_fd, SPI_IOC_MESSAGE(1), &tr_cmd);
};


// configures AnalogDevices 21371 SHARC DSP via SPI
// accepts path to bitstream-file
// returns 0 if sucecssul, -1 on errors
// callit like: spiUploadBitstreamDsps(argv[1], argv[2], 2)
//
// /RESET: 1 -> 0 -> 1 (Linux already inverts the reset for us)
// wait at least 256 us
// Load data:
//   - load Kernel: 1536 8-bit words (equals 256 48-bit Words)
//   - load application (user dependent)
//   - load IVT (1536 8-bit words)
// DMA-Transfer expects a seemless data-transport while the manual says something about handshake and wait-states... strange
//
int SPI::UploadBitstreamDsps(bool useCli) {
    
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
    int last_progress = -1;
    int current_progress;

    uint8_t spiMode = SPI_MODE_3; // AnalogDevices uses MODE 3 (CPOL=1, CPHA=1)
    uint8_t spiBitsPerWord = 32; // we are transmitting in 32-bit-mode during configuration
    uint32_t spiSpeed = 2 * state->dsp_spi_config_speed;
    //uint8_t spiLsbFirst = 0; // Linux-driver for i.MX25 seems to ignore this option, so we flip bitorder manually

    string filename_dsp1 = app->get_option("--D1")->as<string>();
    string filename_dsp2 = app->get_option("--D2")->as<string>();

    // abort if no file path was given for dsp1
    if (filename_dsp1.length() == 0) {
        return -1;
    }

    uint8_t numStreams = 0;
    if (filename_dsp1.length() > 0) {
        numStreams = 1;
    }
    if (filename_dsp2.length() > 0) {
        numStreams = 2;
    }

    // read size of bitstream-files
    file_size[0] = helper->GetFileSize(filename_dsp1.c_str());
    if (file_size[0] <= 0) {
        helper->Error("Problem with bitstream-file\n");
              return -1;
    }
    if (numStreams == 2) {
        file_size[1] = helper->GetFileSize(filename_dsp2.c_str());
        if (file_size[1] <= 0) {
            helper->Error("Problem with bitstream-file\n");
                  return -1;
         }
    }

    helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Connecting to SPI for DSP1...\n");
    spi_fd[0] = open(SPI_DEVICE_DSP1, O_RDWR);
    if (spi_fd[0] < 0) {
        helper->Error("Could not open SPI-device for DSP1\n");
        return -1;
    }
    if (numStreams == 2) {
        helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Connecting to SPI for DSP2...\n");
        spi_fd[1] = open(SPI_DEVICE_DSP2, O_RDWR);
        if (spi_fd[1] < 0) {
            helper->Error("Could not open SPI-device for DSP2\n");
            return -1;
        }
    }
    // SPI-Modus 3 (3 = CPOL=1, CPHA=1)
    ioctl(spi_fd[0], SPI_IOC_WR_MODE, &spiMode);
    ioctl(spi_fd[0], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spi_fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    //ioctl(spi_fd[0], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst); //  // Linux-driver for i.MX25 seems to ignore this option, so we flip bitorder manually
    if (numStreams == 2) {
        ioctl(spi_fd[1], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spi_fd[1], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spi_fd[1], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
        //ioctl(spi_fd[1], SPI_IOC_WR_LSB_FIRST, &spiLsbFirst); //  // Linux-driver for i.MX25 seems to ignore this option, so we flip bitorder manually
    }

    // setup SPI-buffer
    tr.tx_buf = (unsigned long)spiTxData;
    tr.rx_buf = (unsigned long)spiRxData;
    tr.bits_per_word = spiBitsPerWord;
    tr.cs_change = 0;
    tr.delay_usecs = 0;

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
        if (i == 0) {
            bitstream_file[i] = fopen(filename_dsp1.c_str(), "rb");
        }
        if (i == 1) {
            bitstream_file[i] = fopen(filename_dsp2.c_str(), "rb");
        }
        if (!bitstream_file[i]) {
            helper->Error("Could not open bitstream-file\n");
            for (uint8_t i_b = 0; i_b < numStreams; i_b++) {
                if (bitstream_file[i_b]) fclose(bitstream_file[i_b]);
                if (spi_fd[i_b] >= 0) close(spi_fd[i_b]);
            }
            return -1;
        }

        // now send the data
        helper->DEBUG_SPI(DEBUGLEVEL_NORMAL, "Sending bitstream to DSP...\n");
        totalBytesSent = 0;
        last_progress = -1;
        progress_bar_width = 50;
        while ((bytesRead = fread(&spiTxData[0], 1, sizeof(spiTxData), bitstream_file[i])) > 0) {
            tr.len = bytesRead;

            // reverse bitorder as linux-driver seems to have no support for this
            helper->ReverseBitOrderArray(&spiTxData[0], bytesRead);

            ret = ioctl(spi_fd[i], SPI_IOC_MESSAGE(1), &tr);
            if (ret < 0) {
                helper->Error("SPI-transmission failed\n");
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
            if (useCli) {
                totalBytesSent += bytesRead;
                current_progress = (totalBytesSent * 100) / file_size[i];
                if ((current_progress > (last_progress + 5)) || (totalBytesSent == file_size[i])) {
                    int progress = (int)((float)totalBytesSent / file_size[i] * progress_bar_width);
                    printf("\rDSP%u [", i+1);
                    for (int i_progress = 0; i_progress < progress_bar_width; ++i_progress) {
                        if (i_progress < progress) {
                            printf("█");
                        }else{
                            printf(" ");
                        }
                    }
                    printf("] %ld/%ld Bytes (%u %%)", totalBytesSent, file_size[i], (uint)((totalBytesSent * 100) / file_size[i]));
                    fflush(stdout);

                    last_progress = current_progress;
                }
            }else{
                // use UI to show progress
                totalBytesSent += bytesRead;
                current_progress = (totalBytesSent * 100) / file_size[i];
                if ((current_progress > (last_progress + 5)) || (totalBytesSent == file_size[i])) {
                    int progress = (int)((float)totalBytesSent / file_size[i] * 100);

					//pthread_mutex_lock(&lvgl_mutex);
                    lv_bar_set_value(objects.testbar, progress, LV_ANIM_OFF);
					//pthread_mutex_unlock(&lvgl_mutex);
					lv_timer_handler(); // allow LVGL to handle UI

                    last_progress = current_progress;
                }
            }
        }
        
        if (useCli) {
            printf("\n");
        }

        fclose(bitstream_file[i]);
        close(spi_fd[i]);
        usleep(50000); // wait 50ms
    }

    return 0;
}

bool SPI::OpenConnectionFpga() {
    uint8_t spiMode = SPI_MODE_0; // user-program uses SPI MODE 0
    uint8_t spiBitsPerWord = 8; // we are using standard 8-bit-mode here for communication
    uint32_t spiSpeed = SPI_FPGA_SPEED_HZ;

    if (state->raspi)
    {
        spiFpgaHandle = open("/dev/spidev0.0", O_RDWR);
    }
    else
    {
        spiFpgaHandle = open(SPI_DEVICE_FPGA, O_RDWR);
    }

    if (spiFpgaHandle < 0) {
        helper->Error("Error: Could not open SPI-device");
        return false;
    }

    ioctl(spiFpgaHandle, SPI_IOC_WR_MODE, &spiMode);
    ioctl(spiFpgaHandle, SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
    ioctl(spiFpgaHandle, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    return true;
}

bool SPI::CloseConnectionFpga() {
    if (spiFpgaHandle >= 0) {
        close(spiFpgaHandle);
    }
    return true;
}

bool SPI::OpenConnectionDsps() {
    uint8_t spiMode = SPI_MODE_3; // user-program uses SPI MODE 3
    uint8_t spiBitsPerWord = 32; // we are using 32-bit-mode here for communication
    uint32_t spiSpeed = 2 * state->dsp_spi_speed; // Linux SPI driver seems to divide speed by 2

    for (uint8_t i = 0; i < 2; i++) {
        if (i == 0) {
            spiDspHandle[i] = open(SPI_DEVICE_DSP1, O_RDWR);
        }else if (i == 1) {
            spiDspHandle[i] = open(SPI_DEVICE_DSP2, O_RDWR);
        }
        if (spiDspHandle[i] < 0) {
            connected = false;
            helper->Error("Error: Could not open SPI-device\n");            
            return false;
        }

        ioctl(spiDspHandle[i], SPI_IOC_WR_MODE, &spiMode);
        ioctl(spiDspHandle[i], SPI_IOC_WR_BITS_PER_WORD, &spiBitsPerWord);
        ioctl(spiDspHandle[i], SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed);
    }

    // reset RingBuffer-elements
    spiRxRingBuffer[0].head = 0;
    spiRxRingBuffer[0].tail = 0;
    spiRxRingBuffer[1].head = 0;
    spiRxRingBuffer[1].tail = 0;

    spiTxRingBuffer[0].head = 0;
    spiTxRingBuffer[0].level = 0;
    spiTxRingBuffer[1].head = 0;
    spiTxRingBuffer[1].level = 0;

    connected = true;
    return true;
}

bool SPI::CloseConnectionDsps() {
    if (!connected) return false;

    for (uint8_t i = 0; i < 2; i++) {
        if (spiDspHandle[i] >= 0) close(spiDspHandle[i]);
    }
    return true;
}

bool SPI::SendFpgaData(uint8_t txData[], uint8_t rxData[], uint8_t len) {
//    if (!connected) return false; // this line prevents SPI-communication at the moment

    struct spi_ioc_transfer tr = {0};

    // configure SPI-system for this transmission
    tr.tx_buf = (unsigned long)txData;
    tr.rx_buf = (unsigned long)rxData;
    tr.bits_per_word = 8;
    tr.delay_usecs = 0; // microseconds to delay after this transfer before (optionally) changing the chipselect status
    tr.cs_change = 0; // disable CS between two messages
    tr.len = len;
    tr.speed_hz = SPI_FPGA_SPEED_HZ;

    setFpgaChipSelectPin(true); // assert ChipSelect
    int ret = ioctl(spiFpgaHandle, SPI_IOC_MESSAGE(1), &tr); // send via SPI
	setFpgaChipSelectPin(false); // deassert ChipSelect

    return (ret >= 0);
}

void SPI::QueueDspData(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, float values[]) {
/*
    // check if our ringbuffer has at least one element left
    if (spiTxRingBuffer[dsp].level == SPI_TX_BUFFER_SIZE) {
        // fatal error: ringbuffer is full -> throw this message away
        helper->Error("BufferOverflow in DspTxQueue!\n");
        return;
    }
*/
    if (valueCount > SPI_TX_MAX_WORD_COUNT) {
        // fatal error: SPI_TX_MAX_WORD_COUNT is our maximum at the moment
        helper->Error("Attempt to write more than %d values into DspTxQueue!\n", SPI_TX_MAX_WORD_COUNT);
        return;
    }

    // queue element to the buffer
    sSpiTxBufferElement* buffer = &spiTxRingBuffer[dsp].buffer[spiTxRingBuffer[dsp].head];
    buffer->classId = classId;
    buffer->channel = channel;
    buffer->index = index;
    buffer->valueCount = valueCount;
    memcpy(&buffer->values[0], &values[0], valueCount * sizeof(float));
    
    // increment level and head
    spiTxRingBuffer[dsp].level++;
    spiTxRingBuffer[dsp].head++;
    if (spiTxRingBuffer[dsp].head == SPI_TX_BUFFER_SIZE) {
        spiTxRingBuffer[dsp].head = 0;
    }
}

uint32_t SPI::GetDspTxQueueLength(uint8_t dsp) {
    return spiTxRingBuffer[dsp].level;
}

void SPI::ProcessDspTxQueue(uint8_t dsp) {
    // this function is called every 10ms, except once when we are reading from DSP via DMA-Access
    if (spiTxRingBuffer[dsp].level == 0) {
        // nothing to do here
        return;
    }

    int messagesToSend = spiTxRingBuffer[dsp].level;
    // we must not exceed 5ms. Each message can have up to 50 Words with each 32bits
    // so a single message takes a maximum of (1/8MHz) * 50 * 32bit = 0.2ms. So we should
    // not transmit more than 25 messages at once and leave the rest for another timeslot
    // to stay safe, we transmit a maximum of 20 messages per interval

    // calculate how many messages we can transmit in 5ms
    // 1/5ms = 200
    // 8Mhz / 32bit / 50 Words = 5000 Hz -> 0.2ms per message -> max. 25 messages in 5ms
    int maxMessages = (((SPI_DSP_SPEED_HZ / SPI_TX_MAX_WORD_COUNT) / 32) / 200);
    if (messagesToSend > maxMessages) {
        messagesToSend = maxMessages;
    }

    int tail = spiTxRingBuffer[dsp].head - spiTxRingBuffer[dsp].level;
    if (tail < 0) {
        tail += SPI_TX_BUFFER_SIZE;
    }
    for (int i = 0; i < messagesToSend; i++) {
        SendDspData(dsp, &spiTxRingBuffer[dsp].buffer[tail]);
        tail++;
        if (tail == SPI_TX_BUFFER_SIZE) {
            tail = 0;
        }
    }
    spiTxRingBuffer[dsp].level -= messagesToSend;
}

bool SPI::SendDspData(uint8_t dsp, sSpiTxBufferElement* buffer) {

    if (state->bodyless || state->raspi)
    {
        return false;
    }

    if (buffer->valueCount == 0) {
        // dont allow empty messages
        return false;
    }

    struct spi_ioc_transfer tr = {0};
    uint32_t spiTxData[buffer->valueCount + 3]; // '*' + parameter + values + '#'
    uint8_t spiTxDataRaw[(buffer->valueCount + 3) * sizeof(uint32_t)];

    // configure SPI-system for this transmission
    tr.tx_buf = (unsigned long)spiTxDataRaw;
    tr.rx_buf = 0; // we dont want to receive
    tr.bits_per_word = 32;
    tr.delay_usecs = 0; // microseconds to delay after this transfer before (optionally) changing the chipselect status
    tr.cs_change = 0; // disable CS between two messages
    tr.len = sizeof(spiTxDataRaw);

    // prepare TxBuffer
    spiTxData[0] = SPI_START_MARKER; // add StartMarker = '*'
    spiTxData[1] = ((uint32_t)buffer->valueCount << 24) + ((uint32_t)buffer->index << 16) + ((uint32_t)buffer->channel << 8) + (uint32_t)buffer->classId; // some header-information
    memcpy(&spiTxData[2], &buffer->values[0], buffer->valueCount * sizeof(uint32_t));
    spiTxData[(buffer->valueCount + 3) - 1] = SPI_END_MARKER; // add EndMarker = '#'
    memcpy(&spiTxDataRaw[0], &spiTxData[0], sizeof(spiTxDataRaw)); // TODO: check if we can omit the spiTxDataRaw buffer and use only the spiTxData-buffer

    if (helper->DEBUG_SPI(DEBUGLEVEL_TRACE)) {
        printf("SendDspData: ");
        for(int v = 0; v < ((buffer->valueCount + 3) * sizeof(uint32_t)); v++) {
            printf("0x%.2X ", spiTxDataRaw[v]);
            if (!((v+1) % 4)) {
                printf("| ");
            }
        }
        printf("\n");
    }

    UpdateNumberOfExpectedReadBytes(dsp, buffer->classId, buffer->channel, buffer->index);

    int ret = ioctl(spiDspHandle[dsp], SPI_IOC_MESSAGE(1), &tr); // send via SPI
    return (ret >= 0);
}

void SPI::UpdateNumberOfExpectedReadBytes(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index) {
    // we expect only read-class here
    if (classId != '?') {
        return;
    }

    // channel == 0 is a dummy-option and will not be handled here
    if (channel == 0) {
        return;
    }

    // we want to read data from DSP, so check how many bytes we should receive (only payload, without header and tail)
    switch (channel) {
        case 'u': // update-packet
            if (dsp == 0) {
                dataToRead[dsp] = (3 + 40 + 8 + 0 + 3); // DSP1: DspVersion, DspLoad, VolumeDspChan, VolumeFxReturn, VolumeMixBus, VolumeMainLRS
            }else{
                dataToRead[dsp] = 4 + 64; // DSP2: DspVersion, DspHeapSpace, DspLoad, audioGlitchCounter, RTA-Data
            }
            break;
        default:
            break;
    }
}

bool SPI::ReadDspData(uint8_t dsp, uint8_t classId, uint8_t channel, uint8_t index) {
//    if (!connected) return false; // this line prevents SPI-communication at the moment

    // at the moment we are not using the ring-buffer-system, so this is just a simple buffer at the moment
    spiRxRingBuffer[dsp].head = 0;
    spiRxRingBuffer[dsp].tail = 0;
    uint32_t valueCount = dataToRead[dsp];

    if (valueCount == 0) {
        // dont allow empty messages
        return false;
    }

    struct spi_ioc_transfer tr = {0};
    uint32_t spiTxData[valueCount + 3]; // '*' + parameter + values + '#'
    uint8_t spiTxDataRaw[(valueCount + 3) * sizeof(uint32_t)];
    uint8_t spiRxDataRaw[sizeof(spiTxDataRaw)];

    // configure SPI-system for this transmission
    tr.tx_buf = (unsigned long)spiTxDataRaw;
    tr.rx_buf = (unsigned long)spiRxDataRaw;
    tr.bits_per_word = 32;
    tr.delay_usecs = 0; // microseconds to delay after this transfer before (optionally) changing the chipselect status
    tr.cs_change = 0; // disable CS between two messages
    tr.len = sizeof(spiTxDataRaw);

    // prepare TxBuffer
    spiTxData[0] = SPI_START_MARKER; // add StartMarker = '*'
    spiTxData[1] = ((uint32_t)valueCount << 24) + ((uint32_t)index << 16) + ((uint32_t)channel << 8) + (uint32_t)classId; // some header-information
    memset(&spiTxData[2], 0, valueCount * sizeof(uint32_t)); // we are sending zeros, so set the buffer to zero
    spiTxData[(valueCount + 3) - 1] = SPI_END_MARKER; // add EndMarker = '#'
    memcpy(&spiTxDataRaw[0], &spiTxData[0], sizeof(spiTxDataRaw));
    
    if (helper->DEBUG_SPI(DEBUGLEVEL_TRACE)) {
        printf("ReadDspData: ");
        for(int v = 0; v < ((valueCount + 3) * sizeof(uint32_t)); v++) {
            printf("0x%.2X ", spiTxDataRaw[v]);
            if (!((v+1) % 4)) {
                printf("| ");
            }
        }
        printf("\n");
    }

    int32_t bytesRead = ioctl(spiDspHandle[dsp], SPI_IOC_MESSAGE(1), &tr); // send via SPI

    //helper->DEBUG_SPI(DEBUGLEVEL_TRACE, "DSP%d, %d Bytes received", dsp+1, bytesRead);
    PushValuesToRxBuffer(dsp, bytesRead/sizeof(uint32_t), (uint32_t*)spiRxDataRaw);

    return (bytesRead > 0);
}

void SPI::PushValuesToRxBuffer(uint8_t dsp, uint32_t valueCount, uint32_t values[]) {
//    if (!connected) return;

    if (state->bodyless || state->raspi)
    {
        return;
    }


    if (valueCount == 0) {
        return;
    }

    for (int i = 0; i < valueCount; i++) {
        // check for buffer-overflow
        int next_head = spiRxRingBuffer[dsp].head + 1;
        if (next_head > SPI_RX_BUFFER_SIZE) {
            next_head -= SPI_RX_BUFFER_SIZE;
        }
        if (next_head != spiRxRingBuffer[dsp].tail) {
            // no overflow -> store data
            spiRxRingBuffer[dsp].buffer[spiRxRingBuffer[dsp].head] = values[i];
            spiRxRingBuffer[dsp].head = next_head;

            // check for EndMarker '#'
            if (values[i] == SPI_END_MARKER) {
                ProcessRxData(dsp);
            }
        }else{
            // buffer-overflow -> reject new data
            // clear buffer (we receive 0x000000 if no data is sent)
            helper->Log("DSP%d: buffer-overflow -> reject new data", dsp);
            spiRxRingBuffer[dsp].head = 0;
            spiRxRingBuffer[dsp].tail = 0;
            spiRxRingBuffer[dsp].state = LOOKING_FOR_START_MARKER;
        }
    }

    // reduce or reset dataToRead-counter
    if (dataToRead[dsp] >= valueCount) {
        dataToRead[dsp] -= valueCount;
    }else{
        dataToRead[dsp] = 0;
    }
}

void SPI::ProcessRxData(uint8_t dsp) {
//    if (!connected) return;

    while (spiRxRingBuffer[dsp].head != spiRxRingBuffer[dsp].tail) {
        uint32_t data = spiRxRingBuffer[dsp].buffer[spiRxRingBuffer[dsp].tail];
        spiRxRingBuffer[dsp].tail += 1;
        if (spiRxRingBuffer[dsp].tail > SPI_RX_BUFFER_SIZE) {
            spiRxRingBuffer[dsp].tail -= SPI_RX_BUFFER_SIZE;
        }

        switch (spiRxRingBuffer[dsp].state) {
            case LOOKING_FOR_START_MARKER:
                // check for StartMarker
                if (data == SPI_START_MARKER) {
                    spiRxRingBuffer[dsp].payloadIdx = 0;
                    spiRxRingBuffer[dsp].state = COLLECTING_PAYLOAD;
                }
                break;
            case COLLECTING_PAYLOAD:
                if (spiRxRingBuffer[dsp].payloadIdx == 0) {
                    // the current data contains the expected message length
                    spiRxRingBuffer[dsp].payloadLength = ((data & 0xFF000000) >> 24) + 1; // '*' parameter values '#'
                }

                // read data
                spiRxRingBuffer[dsp].payload[spiRxRingBuffer[dsp].payloadIdx++] = data;

                if ((spiRxRingBuffer[dsp].payloadIdx == spiRxRingBuffer[dsp].payloadLength) || (spiRxRingBuffer[dsp].payloadIdx == SPI_MAX_RX_PAYLOAD_SIZE)) {
                    // payload is complete. Now check the end marker
                    spiRxRingBuffer[dsp].state = LOOKING_FOR_END_MARKER;
                }
                break;
            case LOOKING_FOR_END_MARKER:
                // check for the character '#'
                if (data == SPI_END_MARKER) {
                    // we received a valid payload
                    uint8_t classId    =  (spiRxRingBuffer[dsp].payload[0] & 0x000000FF);
                    uint8_t channel    = ((spiRxRingBuffer[dsp].payload[0] & 0x0000FF00) >> 8);
                    uint8_t index      = ((spiRxRingBuffer[dsp].payload[0] & 0x00FF0000) >> 16);
                    uint8_t valueCount = ((spiRxRingBuffer[dsp].payload[0] & 0xFF000000) >> 24);

                    eventBuffer.push_back(new SpiEvent(dsp, classId, channel, index, valueCount, &spiRxRingBuffer[dsp].payload[1]));
                    spiRxRingBuffer[dsp].state = LOOKING_FOR_START_MARKER; // reset state
                }else{
                    // error: end-marker not found
                    spiRxRingBuffer[dsp].state = LOOKING_FOR_START_MARKER; // reset state anyway
                }
                break;
            default:
                break;
        }
    }
}

bool SPI::HasNextEvent(void){
//    if (!connected) return false; // this line prevents SPI-communication at the moment

    return eventBuffer.size() > 0;
}

SpiEvent* SPI::GetNextEvent(void){
    SpiEvent* event = eventBuffer.back();
    eventBuffer.pop_back();
    return event;
}
