#include "wing_touch.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t wing_checksum(const uint8_t *payload, size_t len) {
    unsigned int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum = (sum + payload[i]) & 0xffu;
    }
    return (uint8_t)(((sum & 0xffu) ^ (len & 0xffu)) | 0x80u);
}

WingTouch::WingTouch(State* state) {
    this->state = state;
    this->touch_fd = -1;
    this->running = false;
    this->last_x = 0;
    this->last_y = 0;
    this->is_pressed = false;
    pthread_mutex_init(&this->mutex, NULL);
}

WingTouch::~WingTouch() {
    running = false;
    if (thread) {
        pthread_join(thread, NULL);
    }
    if (touch_fd >= 0) {
        close(touch_fd);
    }
    pthread_mutex_destroy(&this->mutex);
}

int WingTouch::OpenSerial(const char* path) {
    struct termios tio;
    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);
    if (fd < 0) return -1;

    if (tcgetattr(fd, &tio) != 0) {
        close(fd);
        return -1;
    }

    cfmakeraw(&tio);
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);
    tio.c_cflag |= CLOCAL | CREAD;
    tio.c_cflag |= PARENB;
    tio.c_cflag &= ~PARODD;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    return fd;
}

int WingTouch::SendFrame(uint8_t cmd, const uint8_t* payload, size_t len) {
    uint8_t out[1024];
    size_t out_len = 0;
    out[out_len++] = WING_ESCAPE;
    out[out_len++] = cmd;
    for (size_t i = 0; i < len; i++) {
        out[out_len++] = payload[i];
        if (payload[i] == WING_ESCAPE && (i + 1 == len || payload[i + 1] > 0x3fu))
            out[out_len++] = 0x40;
    }
    uint8_t chk = wing_checksum(payload, len);
    out[out_len++] = WING_ESCAPE;
    out[out_len++] = chk;

    size_t written = 0;
    while (written < out_len) {
        ssize_t n = write(touch_fd, out + written, out_len - written);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        written += n;
    }
    return 0;
}

bool WingTouch::Init() {
    touch_fd = OpenSerial("/dev/ttymxc3");
    if (touch_fd < 0) {
        perror("WingTouch: Failed to open UART4 (/dev/ttymxc3)");
        return false;
    }

    const uint8_t touch_on[] = { 0 };
    if (SendFrame('I', touch_on, sizeof(touch_on)) != 0) {
        perror("WingTouch: Failed to send touch enable command");
        close(touch_fd);
        touch_fd = -1;
        return false;
    }

    usleep(100000);
    tcflush(touch_fd, TCIFLUSH);

    running = true;
    if (pthread_create(&thread, NULL, ThreadFunc, this) != 0) {
        perror("WingTouch: Failed to create touch thread");
        running = false;
        close(touch_fd);
        touch_fd = -1;
        return false;
    }

    return true;
}

void WingTouch::GetState(int& x, int& y, bool& pressed) {
    pthread_mutex_lock(&mutex);
    x = last_x;
    y = last_y;
    pressed = is_pressed;
    pthread_mutex_unlock(&mutex);
}

void* WingTouch::ThreadFunc(void* arg) {
    WingTouch* self = (WingTouch*)arg;
    self->Loop();
    return NULL;
}

void WingTouch::Loop() {
    enum DecoderState {
        DEC_WAIT_START,
        DEC_IN_FRAME,
        DEC_AFTER_ESCAPE,
        DEC_WAIT_CHECKSUM,
    } dstate = DEC_WAIT_START;

    uint8_t frame[512];
    size_t flen = 0;
    uint8_t buf[256];

    while (running) {
        ssize_t n = read(touch_fd, buf, sizeof(buf));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                usleep(5000); // 5ms sleep
                continue;
            }
            break;
        }
        if (n == 0) {
            usleep(5000);
            continue;
        }

        for (ssize_t i = 0; i < n; i++) {
            uint8_t byte = buf[i];
            switch (dstate) {
            case DEC_WAIT_START:
                if (byte == WING_ESCAPE) {
                    dstate = DEC_IN_FRAME;
                    flen = 0;
                }
                break;
            case DEC_IN_FRAME:
                if (byte == WING_ESCAPE) {
                    if (flen > 0) {
                        dstate = DEC_AFTER_ESCAPE;
                    } else {
                        flen = 0;
                    }
                } else {
                    if (flen < sizeof(frame)) frame[flen++] = byte;
                    else { dstate = DEC_WAIT_START; flen = 0; }
                }
                break;
            case DEC_AFTER_ESCAPE:
                if (byte == 0x40) {
                    if (flen < sizeof(frame)) frame[flen++] = WING_ESCAPE;
                    else { dstate = DEC_WAIT_START; flen = 0; }
                    dstate = DEC_IN_FRAME;
                } else if ((byte & 0x80u) != 0) {
                    dstate = DEC_WAIT_CHECKSUM;
                    if (flen >= 1) {
                        uint8_t cmd = frame[0];
                        uint8_t* payload = frame + 1;
                        size_t plen = flen - 1;
                        uint8_t expected = wing_checksum(payload, plen);
                        if (byte == expected) {
                            if (cmd == 'p' && plen == 5) {
                                uint8_t kind = payload[0];
                                bool pressed = (kind & 0xf0) != 0x10;
                                int rx = payload[1] | (payload[2] << 8);
                                int ry = payload[3] | (payload[4] << 8);

                                pthread_mutex_lock(&mutex);
                                last_x = rx;
                                last_y = ry;
                                is_pressed = pressed;
                                pthread_mutex_unlock(&mutex);
                            }
                        }
                    }
                    dstate = DEC_WAIT_START;
                    flen = 0;
                } else {
                    if (flen < sizeof(frame)) frame[flen++] = WING_ESCAPE;
                    else { dstate = DEC_WAIT_START; flen = 0; }
                    dstate = DEC_IN_FRAME;
                    if (flen < sizeof(frame)) frame[flen++] = byte;
                    else { dstate = DEC_WAIT_START; flen = 0; }
                }
                break;
            case DEC_WAIT_CHECKSUM:
                dstate = DEC_WAIT_START;
                flen = 0;
                break;
            }
        }
    }
}
