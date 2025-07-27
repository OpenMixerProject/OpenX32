// this software reads the MAC-address from the internal X32-SD-Card and configures the ethernet-interface
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
#include "../../linux/include/uapi/linux/if_arp.h"

#define MMC_DEVICE "/dev/mmcblk0"
#define SD_READ_OFFSET_START 0x1B0
#define SD_READ_OFFSET_END 0x27F

int main() {
    int fd;
    unsigned char buffer[SD_READ_OFFSET_END - SD_READ_OFFSET_START + 1];
    ssize_t bytes_read;
    unsigned int *value_ptr;
    
    // check if sd-card is available
    fd = open(MMC_DEVICE, O_RDONLY);
    if (fd == -1) {
        // no SD-card available -> do nothing
        return EXIT_FAILURE;
    }
    
    // read data
    off_t seek_result = lseek(fd, SD_READ_OFFSET_START, SEEK_SET);
    if (seek_result == -1) {
        // cannot access SD-card
        close(fd);
        return EXIT_FAILURE;
    }
    
    bytes_read = read(fd, buffer, sizeof(buffer));
    close(fd); // we do not need the SD-card anymore, so close it
    if ((bytes_read == -1) || (bytes_read != sizeof(buffer))) {
        // error reading data
        return EXIT_FAILURE;
    }
    
    // check for expected 0xFEEDBEEF
    uint32_t magic_number = ((uint32_t)buffer[0x08] << 24) + ((uint32_t)buffer[0x09] << 16) + ((uint32_t)buffer[0x0A] << 8) + ((uint32_t)buffer[0x0B]);
    if (magic_number != 0xFEEDBEEF) {
        // unexpected data
        return EXIT_FAILURE;
    }
    
    // now configure the network-interface
    char *interface_name = "eth0";
    struct ifreq ifr;
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        // cannot create socket
        return EXIT_FAILURE;
    }
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    // turnoff interface
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == -1) {
        // error on reading interface-flags
        close(sock);
        return EXIT_FAILURE;
    }
    ifr.ifr_flags &= ~IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1) {
        // error shutting down interface
        close(sock);
        return EXIT_FAILURE;
    }
    
    // set MAC-address and start interface again
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, buffer + 0xBB, 6);
    if (ioctl(sock, SIOCSIFHWADDR, &ifr) == -1) {
        // error setting the MAC-address
        close(sock);
        return EXIT_FAILURE;
    }
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sock, SIOCSIFFLAGS, &ifr) == -1) {
        // error starting the interface again
        close(sock);
        return EXIT_FAILURE;
    }
    close(sock);
    
    printf("Configured MAC-address to %02x:%02x:%02x:%02x:%02x:%02x\n",
           buffer[0xBB], buffer[0xBB + 1],
           buffer[0xBB + 2], buffer[0xBB + 3],
           buffer[0xBB + 4], buffer[0xBB + 5]);
    
    return EXIT_SUCCESS;
}