/* 
 * This example shows how to use GPIO ports to get logical "1" or "0".
 * LED is attached to PE8. 
 */

#include "usart.h"
#include "stm32f3xx.h"

static USART_TypeDef* usart = USART2;

void rcc_init()
{
	RCC->AHBENR     |= RCC_AHBENR_GPIOAEN;
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
    usart->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; 
}

int main()
{
	char c;
   
    rcc_init();
    usart_init();
    
    while (1) {
        c = usart_recv_char_sync(usart);
        usart_send_char_sync(usart, c);
    }
}

