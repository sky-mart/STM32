#include "usart.h"
#include "gpio.h"
#include "dma.h"

void usart_gpio_init(gpio_pin_t* tx_pin, gpio_pin_t* rx_pin)
{
	gpio_pin_set_mode(tx_pin, GPIO_MODE_ALTER);
	gpio_pin_set_otype(tx_pin, GPIO_OTYPE_PUSHPULL);
	gpio_pin_set_ospeed(tx_pin, GPIO_OSPEED_HIGH);
	gpio_pin_set_pupd(tx_pin, GPIO_PUPD_PULLUP);
	gpio_pin_set_altfun(tx_pin);

	gpio_pin_set_mode(rx_pin, GPIO_MODE_ALTER);
	gpio_pin_set_otype(rx_pin, GPIO_OTYPE_PUSHPULL);
	gpio_pin_set_ospeed(rx_pin, GPIO_OSPEED_HIGH);
	gpio_pin_set_pupd(rx_pin, GPIO_PUPD_PULLUP);
	gpio_pin_set_altfun(rx_pin);
}

void usart_send_char_sync(USART_TypeDef *usart, char c)
{
	while (!(usart->ISR & USART_ISR_TXE)); 
    usart->TDR = c; 
}

char usart_recv_char_sync(USART_TypeDef *usart)
{
	while (!(usart->ISR & USART_ISR_RXNE)); 
    return usart->RDR;
}

void usart_send_async_dma(USART_TypeDef *usart, DMA_Channel_TypeDef* dma_channel, const char* data, uint32_t size)
{
	dma_transfer_t dma_trasfer;
	dma_trasfer.memory_addr = (void*)data;
	dma_trasfer.periph_addr = (void*)&usart->TDR;
	dma_trasfer.count = size;
	*(uint32_t*)&dma_trasfer.config = 0U;
	dma_trasfer.config.DIR = 1;
	dma_trasfer.config.MINC = 1;
	dma_channel_init(dma_channel, &dma_trasfer);
}