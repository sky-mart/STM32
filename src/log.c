#include "log.h"
#include "gpio.h"
#include "usart.h"
#include "stm32f3xx.h"
#include <stdarg.h>
#include <stdio.h>

static USART_TypeDef* log_usart = USART2;
static gpio_pin_t log_usart_tx_pin = {GPIOA, 2, 7};	// PA2, AF7
static gpio_pin_t log_usart_rx_pin = {GPIOA, 3, 7};	// PA3, AF7
static DMA_Channel_TypeDef* log_usart_dma_channel = DMA1_Channel7;
static IRQn_Type dma_channel_irq = DMA1_Channel7_IRQn;

#define LOG_SIZE 20UL

static char log_buffer[LOG_SIZE + 1];
static char* log_cur_data = 0;
static uint32_t log_cur_size = 0;
static char* log_next_data = 0;
static uint32_t log_next_size = 0;

static void log_usart_init()
{
	usart_gpio_init(&log_usart_tx_pin, &log_usart_rx_pin);
	log_usart->BRR = 0x341; // 8 MHz, 9600 baud/s 
	log_usart->CR3 |= USART_CR3_DMAT; // enable tx dma
    log_usart->CR1 |= 
    	USART_CR1_TE |	// enable trasmitter
    	USART_CR1_RE |	// enable receiver 
    	USART_CR1_UE; 	// enable USART
}

static inline void log_exec_cur_transfer()
{
	usart_send_async_dma(log_usart, log_usart_dma_channel, 1, log_cur_data, log_cur_size);
}

void log_init()
{
	log_usart_init();
	NVIC_EnableIRQ(dma_channel_irq);
}

/*            buffer
 * |--------------------------|
 *     ^        ^         ^ 
 *     |        |         |
 *      cur_data next_data
 */

int log_printf(const char* format, ...)
{
	int len = 0;
	uint32_t available_len = 0;
	char* end = 0;
	va_list va;

	va_start(va, format);
	NVIC_DisableIRQ(dma_channel_irq);

	if (log_cur_size == 0) {
		end = log_buffer;
		len = vsnprintf(end, LOG_SIZE, format, va);
		if (len >= 0) {
			log_cur_data = end;
			log_cur_size = MIN(len, LOG_SIZE);
			log_exec_cur_transfer();
		}
	} else if (log_next_size == 0) {
		end = log_cur_data + log_cur_size;
		available_len = LOG_SIZE - (end - log_buffer);
		len = vsnprintf(end, available_len, format, va);
		if (len >= 0) {
			log_next_data = end;
			log_next_size = MIN(len, available_len);
		}
	} else {
		end = log_next_data + log_next_size;
		available_len = LOG_SIZE - (end - log_buffer);
		len = vsnprintf(end, available_len, format, va);
		if (len >= 0) {
			log_next_size += MIN(len, available_len);
		}
	}

	NVIC_EnableIRQ(dma_channel_irq);
	va_end(va);
	return len;
}

void DMA1_Channel7_IRQHandler()
{
	if (DMA1->ISR & DMA_ISR_TCIF7) {
		DMA1->IFCR |= DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7;
		log_usart_dma_channel->CCR &= ~DMA_CCR_EN;

		if (log_next_size > 0) {
			log_cur_data = log_next_data;
			log_cur_size = log_next_size;
			log_next_data = 0;
			log_next_size = 0;
			log_exec_cur_transfer();
		} else {
			log_cur_data = 0;
			log_cur_size = 0;
		}
	}
}