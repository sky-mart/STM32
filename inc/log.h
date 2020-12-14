#ifndef LOG_H
#define LOG_H

#include "stdint.h"

void log_init();
void log_write(const char* data, uint32_t size);

#endif // LOG_H