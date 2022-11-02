#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define HZ_PER_USEC 1000000 / HZ

void preempt_disable(void) {
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGVTALRM);
  sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void) {
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss, SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void signal_handler() { uthread_yield(); }

void preempt_start(bool preempt) {
  if (!preempt)
    return;
  struct sigaction sa;

  /* Set up handler for alarm */
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGVTALRM, &sa, NULL);

  /* Configure alarm to go off at 100 times per second */
  /* second => microsecond == 1 => 1000000 */
  struct itimerval new;
  new.it_interval.tv_usec = (long int)HZ_PER_USEC;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = (long int)HZ_PER_USEC;
  new.it_value.tv_sec = 0;

  if (setitimer(ITIMER_VIRTUAL, &new, NULL) < 0)
    exit(1);
}

void preempt_stop(void) {
  struct sigaction sa;

  setitimer(ITIMER_VIRTUAL, NULL, NULL);

  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGVTALRM, &sa, NULL);
}
