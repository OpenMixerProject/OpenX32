#include "auxiliary.h"

// variables
timer_t timerid;
struct sigevent sev;
struct itimerspec trigger;
sigset_t mask;

// initialize timer
int initTimer() {
  // Set up the signal handler
  struct sigaction sa;
  sa.sa_handler = timer10msCallback;
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

  // Set the timer to trigger every 10ms
  trigger.it_value.tv_sec = 0;
  trigger.it_value.tv_nsec = 10000000; // 10ms = 10000us = 10000000ns
  trigger.it_interval.tv_sec = 0;
  trigger.it_interval.tv_nsec = 10000000;

  // Arm the timer
  if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
    perror("timer_settime");
    return 1;
  }
}
