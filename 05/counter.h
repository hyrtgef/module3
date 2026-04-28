#ifndef COUNTER_H
#define COUNTER_H

#include <signal.h>
#include <stdio.h>

extern volatile sig_atomic_t keep_running;
extern volatile sig_atomic_t sigint_count;
extern FILE *output_file;

void signal_handler(int signum);

#endif 