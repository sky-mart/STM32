#ifndef LOG_H
#define LOG_H

#include "stdint.h"

void log_init();

typedef enum {
	LOG_PRINTF_SUCCESS = 0,
	LOG_PRINTF_ENCODING_ERROR = 1,
	LOG_PRINTF_PARTIAL_PRINT = 2
} log_printf_result_t;

log_printf_result_t log_printf(const char* format, ...);

#endif // LOG_H