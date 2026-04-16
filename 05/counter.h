#ifndef COUNTER_H
#define COUNTER_H

#include <signal.h>
#include <stdio.h>

// Глобальные переменные для обработчика сигналов
extern volatile sig_atomic_t keep_running;
extern volatile sig_atomic_t sigint_count;
extern FILE *output_file;

// Прототип обработчика сигналов
void signal_handler(int signum);

#endif // COUNTER_H