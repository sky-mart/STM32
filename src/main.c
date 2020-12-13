/* 
 * This example shows how to use GPIO ports to get logical "1" or "0".
 * LED is attached to PE8. 
 */

#include "usart.h"
#include "stm32f3xx.h"
#include "stdio.h"

static USART_TypeDef* usart = USART2;
static DMA_Channel_TypeDef* usart_dma_channel = DMA1_Channel7;

void rcc_init()
{
	RCC->AHBENR     |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_DMA1EN;
    RCC->APB1ENR    |= RCC_APB1ENR_USART2EN;
}

void usart_init()
{
	gpio_pin_t tx_pin, rx_pin;

	tx_pin.gpio = GPIOA;
	tx_pin.pin = 2;
	tx_pin.altfun = 7; 
	rx_pin.gpio = GPIOA;
	rx_pin.pin = 3;
	rx_pin.altfun = 7; 

	usart_gpio_init(&tx_pin, &rx_pin);
	usart->BRR = 0x341; // 8 MHz, 9600 baud/s 
	usart->CR3 |= USART_CR3_DMAT; // enable tx dma
    usart->CR1 |= 
    	USART_CR1_TE |	// enable trasmitter
    	USART_CR1_RE |	// enable receiver 
    	USART_CR1_UE; 	// enable USART
}

int main()
{
	const int MAX_RESP_LEN = 50; 
	char resp[MAX_RESP_LEN];
	int resp_len = 0;
	char c = 0;
   
    rcc_init();
    usart_init();
    
    while (1) {
        c = usart_recv_char_sync(usart);
        resp_len = snprintf(resp, MAX_RESP_LEN, "received char: %c\r\n", c);
        usart_send_async_dma(usart, usart_dma_channel, resp, resp_len);
        // usart_send_char_sync(usart, c);
    }
}

