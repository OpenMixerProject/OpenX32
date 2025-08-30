// This software reads the MAC-address from the internal X32-SD-Card and configures the ethernet-interface
// it also stores all configuration-options to the file /etc/x32.conf for further usage
//
// Header:
// Address 0x1B8
// Data 0xFEEDBEEF
//
// Configuration-Header:
// Address 0x1FE
// Data 0x55AA
//
// Configuration:
// Address 0x200
// ASCII :CFG8D1F:SN=SyymmxxxASF:MDL=X32:DATE=yyyymmdd-hhmmss:DBG=Y:LCD=E0012003,8100EAC6,2BD00000,17150004,128:MAC=aabbccddeeff

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <netinet/ether.h>

#define MMC_DEVICE "/dev/mmcblk0"
#define SD_READ_OFFSET_START 0x1B0
#define SD_READ_OFFSET_END 0x27F

int main() {
    int fd;
    unsigned char buffer[SD_READ_OFFSET_END - SD_READ_OFFSET_START + 1];
    ssize_t bytes_read;
    unsigned int *value_ptr;

    printf("x32sdconfig started\n");

    // check if sd-card is available
    fd = open(MMC_DEVICE, O_RDONLY);
    if (fd == -1) {
        // no SD-card available -> do nothing
        printf("ERROR: no sdcard available\n");
        return EXIT_FAILURE;
    }

    // read data
    off_t seek_result = lseek(fd, SD_READ_OFFSET_START, SEEK_SET);
    if (seek_result == -1) {
        // cannot access SD-card
        close(fd);
        printf("ERROR: cannot access SD-card\n");
        return EXIT_FAILURE;
    }

    bytes_read = read(fd, buffer, sizeof(buffer));
    close(fd); // we do not need the SD-card anymore, so close it
    if ((bytes_read == -1) || (bytes_read != sizeof(buffer))) {
        // error reading data
        printf("ERROR: error reading data\n");
        return EXIT_FAILURE;
    }

    // check for expected 0xFEEDBEEF
    uint32_t magic_number = ((uint32_t)buffer[0x08] << 24) + ((uint32_t)buffer[0x09] << 16) + ((uint32_t)buffer[0x0A] << 8) + ((uint32_t)buffer[0x0B]);
    if (magic_number != 0xFEEDBEEF) {
        // unexpected data
        printf("ERROR: unexpected data\n");
        return EXIT_FAILURE;
    }

    // now configure the network-interface
    char *interface_name = "eth0";
    struct ifreq ifr;
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        // cannot create socket
        printf("ERROR: cannot create socket\n");
        return EXIT_FAILURE;
    }
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // turnoff interface
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1) {
        // error on reading interface-flags
        printf("ERROR: reading interface-flags\n");
        close(sock);
        return EXIT_FAILURE;
    }
    ifr.ifr_flags &= ~IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1) {
        // error shutting down interface
        printf("ERROR: error shutting down interface\n");
        close(sock);
        return EXIT_FAILURE;
    }

    // convert string to MAC-address
    uint8_t mac_addr[6];
    char byte_str[3];
    char *endptr;
    for (uint8_t i=0; i<6; i++) {
        byte_str[0] = buffer[0xBB + (i*2)];
        byte_str[1] = buffer[0xBB + (i*2) + 1];
        byte_str[2] = '\0';
        mac_addr[i] = strtol(byte_str, &endptr, 16);
    }

    // set MAC-address and start interface again
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, mac_addr, 6);
    if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1) {
        // error setting the MAC-address
        printf("ERROR: error setting the MAC-address\n");
        close(sock);
        return EXIT_FAILURE;
    }
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1) {
        // error starting the interface again
        close(sock);
        printf("ERROR: error starting the interface again\n");
        return EXIT_FAILURE;
    }
    close(sock);


    printf("Configured MAC-address to %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_addr[0], mac_addr[1],
           mac_addr[2], mac_addr[3],
           mac_addr[4], mac_addr[5]);


    // write config to /etc/x32.conf
    FILE *file = NULL;
    file = fopen("/etc/x32.conf", "w");
    if (file == NULL) {
        printf("Error creating file");
        return EXIT_FAILURE;
    }
    char *buffer_copy = NULL;
    char *token = NULL;
    char *saveptr = NULL;
    buffer_copy = strdup(&buffer[0x50]);
    if (buffer_copy == NULL) {
        return EXIT_FAILURE;
    }


    // we expecting the following string:
    // :CFG8D1F:SN=SyymmxxxASF:MDL=X32:DATE=yyyymmdd-hhmmss:DBG=Y:LCD=E0012003,8100EAC6,2BD00000,17150004,128:MAC=aabbccddeeff
    // find first config-entry
    token = strtok_r(buffer_copy, ":", &saveptr);
    while (token != NULL) {
//        if (strncmp(token, "CFG", 3) == 0 ||
//            strncmp(token, "SN=", 3) == 0 ||
//            strncmp(token, "MDL=", 4) == 0 ||
//            strncmp(token, "DATE=", 5) == 0) {

//            if (fprintf(file, "%s\n", token) < 0) {
//                return EXIT_FAILURE;
//            }
//        }

        // write all values to config-file
        if (fprintf(file, "%s\n", token) < 0) {
            printf("Error wrinting values to config-file");
            return EXIT_FAILURE;
        }
        // find next config-entry
        token = strtok_r(NULL, ":", &saveptr);
    }
    fclose(file);

    return EXIT_SUCCESS;
}
