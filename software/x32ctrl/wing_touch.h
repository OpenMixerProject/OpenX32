#pragma once

#include <stdint.h>
#include <pthread.h>
#include "state.h"

#define WING_ESCAPE 0x2a

class WingTouch {
    public:
        WingTouch(State* state);
        ~WingTouch();
        bool Init();
        void GetState(int& x, int& y, bool& pressed);

    private:
        State* state;
        int touch_fd;
        pthread_t thread;
        bool running;
        int last_x;
        int last_y;
        bool is_pressed;
        pthread_mutex_t mutex;

        static void* ThreadFunc(void* arg);
        void Loop();
        int OpenSerial(const char* path);
        int SendFrame(uint8_t cmd, const uint8_t* payload, size_t len);
};
