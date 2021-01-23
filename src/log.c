#include "log.h"

#include <stdarg.h>
#include <stdio.h>

#include "gpio.h"
#include "stm32f3xx.h"
#include "usart.h"

#define LOG_BUFFER_SIZE 201UL

typedef struct {
    char* cur_data;
    uint32_t cur_size;
    char* next_data;
    uint32_t next_size;

    USART_TypeDef* usart;
    gpio_pin_t tx_pin;
    gpio_pin_t rx_pin;
    DMA_Channel_TypeDef* dma_channel;
    IRQn_Type dma_channel_irq;

    char buffer[LOG_BUFFER_SIZE];
} log_t;

static log_t log = {
    0,                  // cur_data
    0,                  // cur_size
    0,                  // next_data
    0,                  // next_size
    USART1,             // usart
    {GPIOA, 9, 7},      // tx_pin PA9, AF7
    {GPIOA, 10, 7},     // rx_pin PA10, AF7
    DMA1_Channel4,      // dma_channel
    DMA1_Channel4_IRQn  // dma_channel_irq
};

static void log_usart_init()
{
    usart_gpio_init(&log.tx_pin, &log.rx_pin);
    // log.usart->BRR = 0x4E1;	// 72 MHz, 115200 baud/s, oversampling 8
    log.usart->BRR = 0x85;               // 8 MHz, 115200 baud/s, oversampling 8
    log.usart->CR3 |= USART_CR3_DMAT |   // enable tx dma
                      USART_CR3_OVRDIS;  // disable overrun detection
    log.usart->CR1 |= USART_CR1_OVER8 |  // oversampling 8
                      USART_CR1_TE |     // enable trasmitter
                      USART_CR1_RE |     // enable receiver
                      USART_CR1_UE;      // enable USART
}

static inline void log_exec_cur_transfer()
{
    usart_send_async_dma(log.usart, log.dma_channel, 1, log.cur_data,
                         log.cur_size);
}

void log_init()
{
    log_usart_init();
    NVIC_EnableIRQ(log.dma_channel_irq);
}

/*            buffer
 * |--------------------------|
 *     ^        ^         ^
 *     |        |         |
 *  cur_data next_data   end
 */

log_printf_result_t log_printf(const char* format, ...)
{
    log_printf_result_t result = LOG_PRINTF_SUCCESS;
    int size                   = 0;
    uint32_t available_size    = 0;
    uint32_t existing_size     = 0;
    char* end                  = 0;
    char** to_assign_end       = 0;
    uint32_t* to_assign_size   = 0;
    va_list va;

    va_start(va, format);

    // We need to disable interrupts to use log_printf in interrupt handlers
    __disable_irq();

    if (log.cur_size == 0) {
        end            = log.buffer;
        available_size = LOG_BUFFER_SIZE;
        to_assign_end  = &log.cur_data;
        to_assign_size = &log.cur_size;
    } else if (log.next_size == 0) {
        end            = log.cur_data + log.cur_size;
        available_size = LOG_BUFFER_SIZE - (end - log.buffer);
        to_assign_end  = &log.next_data;
        to_assign_size = &log.next_size;
    } else {
        end            = log.next_data + log.next_size;
        available_size = LOG_BUFFER_SIZE - (end - log.buffer);
        to_assign_size = &log.next_size;
        existing_size  = log.next_size;
    }

    size = vsnprintf(end, available_size, format, va);
    if (size >= 0) {
        if (to_assign_end)
            *to_assign_end = end;

        if (size >= available_size) {
            *to_assign_size = existing_size + available_size - 1;
            result          = LOG_PRINTF_PARTIAL_PRINT;
        } else {
            *to_assign_size = existing_size + size;
        }

        if (to_assign_size == &log.cur_size)
            log_exec_cur_transfer();
    } else {
        result = LOG_PRINTF_ENCODING_ERROR;
    }

    __enable_irq();
    va_end(va);
    return result;
}

void DMA1_Channel4_IRQHandler()
{
    if (DMA1->ISR & DMA_ISR_TCIF4) {
        DMA1->IFCR |= DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4;
        log.dma_channel->CCR &= ~DMA_CCR_EN;

        // We need to disable interrupts to use log_printf in interrupt handlers
        // TODO: think about irq priorities
        __disable_irq();

        if (log.next_size > 0) {
            log.cur_data  = log.next_data;
            log.cur_size  = log.next_size;
            log.next_data = 0;
            log.next_size = 0;
            log_exec_cur_transfer();
        } else {
            log.cur_data = 0;
            log.cur_size = 0;
        }

        __enable_irq();
    }
}