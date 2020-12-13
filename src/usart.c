#include "usart.h"
#include "gpio.h"

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