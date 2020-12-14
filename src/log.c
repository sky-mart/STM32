#include "log.h"
#include "gpio.h"
#include "usart.h"
#include "stm32f3xx.h"

static USART_TypeDef* log_usart = USART2;
static gpio_pin_t log_usart_tx_pin = {GPIOA, 2, 7};	// PA2, AF7
static gpio_pin_t log_usart_rx_pin = {GPIOA, 3, 7};	// PA3, AF7
static DMA_Channel_TypeDef* log_usart_dma_channel = DMA1_Channel7;

void DMA1_Channel7_IRQHandler()
{
	__disable_irq();
	if (DMA1->ISR & DMA_ISR_TCIF7) {
		DMA1->IFCR |= DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7;
		log_usart_dma_channel->CCR &= ~DMA_CCR_EN;
	}
	__enable_irq();
}

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

void log_init()
{
	log_usart_init();
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

void log_write(const char* data, uint32_t size)
{
	__disable_irq();
	// check if previous trasfer has completed
	// and put data in the queue if it has not
	usart_send_async_dma(log_usart, log_usart_dma_channel, 1, data, size);
	__enable_irq();
}