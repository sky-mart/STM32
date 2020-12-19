#include "log.h"
#include "gpio.h"
#include "usart.h"
#include "stm32f3xx.h"
#include <stdarg.h>
#include <stdio.h>

#define LOG_BUFFER_SIZE 21UL

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
	0, // cur_data
	0, // cur_size
	0, // next_data
	0, // next_size
	USART2, // usart
	{GPIOA, 2, 7}, // tx_pin PA2, AF7
	{GPIOA, 3, 7}, // rx_pin PA3, AF7
	DMA1_Channel7, // dma_channel
	DMA1_Channel7_IRQn // dma_channel_irq
};

static void log_usart_init()
{
	usart_gpio_init(&log.tx_pin, &log.rx_pin);
	log.usart->BRR = 0x4E1;				// 72 MHz, 115200 baud/s, oversampling 8
	log.usart->CR3 |= USART_CR3_DMAT;	// enable tx dma
    log.usart->CR1 |= 
    	USART_CR1_OVER8 |	// oversampling 8
    	USART_CR1_TE |		// enable trasmitter
    	USART_CR1_RE |		// enable receiver 
    	USART_CR1_UE;		// enable USART
}

static inline void log_exec_cur_transfer()
{
	usart_send_async_dma(log.usart, log.dma_channel, 1, log.cur_data, log.cur_size);
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

void log_printf(const char* format, ...)
{
	int len = 0;
	uint32_t available_len = 0;
	char* end = 0;
	va_list va;

	va_start(va, format);

	// We need to disable interrupts to use log_printf in interrupt handlers
	__disable_irq();

	if (log.cur_size == 0) {
		end = log.buffer;
		len = vsnprintf(end, LOG_BUFFER_SIZE, format, va);
		if (len >= 0) {
			log.cur_data = end;
			log.cur_size = MIN(len, LOG_BUFFER_SIZE - 1);
			log_exec_cur_transfer();
		}
	} else if (log.next_size == 0) {
		end = log.cur_data + log.cur_size;
		available_len = LOG_BUFFER_SIZE - (end - log.buffer);
		len = vsnprintf(end, available_len, format, va);
		if (len >= 0) {
			log.next_data = end;
			log.next_size = MIN(len, available_len - 1);
		}
	} else {
		end = log.next_data + log.next_size;
		available_len = LOG_BUFFER_SIZE - (end - log.buffer);
		len = vsnprintf(end, available_len, format, va);
		if (len >= 0) {
			log.next_size += MIN(len, available_len - 1);
		}
	}

	__enable_irq();
	va_end(va);
}

void DMA1_Channel7_IRQHandler()
{
	if (DMA1->ISR & DMA_ISR_TCIF7) {
		DMA1->IFCR |= DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7;
		log.dma_channel->CCR &= ~DMA_CCR_EN;

		if (log.next_size > 0) {
			log.cur_data = log.next_data;
			log.cur_size = log.next_size;
			log.next_data = 0;
			log.next_size = 0;
			log_exec_cur_transfer();
		} else {
			log.cur_data = 0;
			log.cur_size = 0;
		}
	}
}