#ifndef LOG_H
#define LOG_H

#include "stdint.h"

void log_init();
int log_printf(const char* format, ...);

#endif // LOG_H