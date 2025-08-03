#include "osc.h"


in_addr_t oscDestrinationIp;
char oscBuffer[2048]; // declare a 2Kb buffer to read packet data into
int oscFd;

void oscInit(char* destIp){
    oscDestrinationIp = inet_addr(destIp);
     oscFd = socket(AF_INET, SOCK_DGRAM, 0);
    fcntl(oscFd, F_SETFL, O_NONBLOCK); // set the socket to non-blocking
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9000);
    sin.sin_addr.s_addr = INADDR_ANY;
    bind(oscFd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
    printf("OpenX32 is now listening on port 9000.\n");

}

void oscRun(){

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(oscFd, &readSet);
    struct timeval timeout = {0, 0}; 
    if (select(oscFd+1, &readSet, NULL, NULL, &timeout) > 0) {
      struct sockaddr sa; // can be safely cast to sockaddr_in
      socklen_t sa_len = sizeof(struct sockaddr_in);
      int len = 0;
      while ((len = (int) recvfrom(oscFd, oscBuffer, sizeof(oscBuffer), 0, &sa, &sa_len)) > 0) {
       
        if (tosc_isBundle(oscBuffer)) {
          tosc_bundle bundle;
          tosc_parseBundle(&bundle, oscBuffer, len);
          const uint64_t timetag = tosc_getTimetag(&bundle);
          tosc_message osc;
          while (tosc_getNextMessage(&bundle, &osc)) {
            processMessage(&osc);
          }
        } else {
          tosc_message osc;
          tosc_parseMessage(&osc, oscBuffer, len);
          processMessage(&osc);
        }
      }
    }

    
}

bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}







void oscSend(char* address, char* datatype, float data)
{
    // declare a buffer for writing the OSC packet into
    char buffer[1024];

    struct sockaddr_in sout;
    
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0){
        perror("cannot open socket");
        return false;
    }
    
    sout.sin_family = AF_INET;
    sout.sin_port = htons(8000);
    sout.sin_addr.s_addr = oscDestrinationIp;

    int len = tosc_writeMessage(buffer, sizeof(buffer), address, datatype, data);

    //                        +1 to include terminator
    if (sendto(fd, buffer, len+1, 0, &sout, sizeof(sout)) < 0){
        perror("cannot send message");
        close(fd);
        return false;
    }

    printf("oscSend: %s %s %f\n\n", buffer, datatype, data);

    close(fd);
}