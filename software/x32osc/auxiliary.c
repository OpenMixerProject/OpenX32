#include "auxiliary.h"

int readConfig(const char *filename, const char *key, char *value_buffer, size_t buffer_size) {
    FILE *file = NULL;
    char line[64];
    int found = -2;

    // open file
    file = fopen(filename, "r");
    if (file == NULL) {
        // no configuration-file
        strncpy(value_buffer, "Mysterious", buffer_size);
        return -1;
    }

    size_t key_len = strlen(key);
    char search_pattern[key_len + 2];
    snprintf(search_pattern, sizeof(search_pattern), "%s", key);

    // read file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            char *value_start = line + strlen(search_pattern);

            // remove spaces (if any)
            while (*value_start == ' ' || *value_start == '\t') {
                value_start++;
            }

            // remove unwanted characters from the end
            char *newline_pos = strchr(value_start, '\n');
            if (newline_pos != NULL) {
                *newline_pos = '\0';
            }
            char *cr_pos = strchr(value_start, '\r');
            if (cr_pos != NULL) {
                *cr_pos = '\0';
            }

            // copy value to buffer
            size_t value_len = strlen(value_start);
            if (value_len >= buffer_size) {
                found = -3; // buffer to small
                break;
            }
            strncpy(value_buffer, value_start, buffer_size - 1);
            value_buffer[buffer_size - 1] = '\0';
            found = 0; // success
            break; // value found. End loop
        }
    }
    fclose(file);

	return found;
}

/*
int valueToBinaryString(uint8_t value, char *output) {
    if (value > 15) {
        return -1;
    }

    // zero-terminate string
    output[4] = '\0';

    for (int i = 3; i >= 0; --i) {
        if ((value >> i) & 1) {
            output[3 - i] = '1';
        } else {
            output[3 - i] = '0';
        }
    }

    return 0;
}
*/

/*
// variables
timer_t timerid;
struct sigevent sev;
struct itimerspec trigger;
sigset_t mask;

// initialize timer
int initTimer() {
  // Set up the signal handler
  struct sigaction sa;
  sa.sa_handler = timerCallback;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  // Set up the sigevent structure for the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;

  // Create the timer
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
    perror("timer_create");
    return 1;
  }

  // Set the timer to trigger every 1 second (1,000,000,000 nanoseconds)
  trigger.it_value.tv_sec = 0;
  trigger.it_value.tv_nsec = 50000000; // 50ms = 50000us = 50000000ns
  trigger.it_interval.tv_sec = 0;
  trigger.it_interval.tv_nsec = 50000000;

  // Arm the timer
  if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
    perror("timer_settime");
    return 1;
  }
}
*/
