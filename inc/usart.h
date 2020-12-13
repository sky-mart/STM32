#ifndef USART_H
#define USART_H

#include "gpio.h"
#include "stm32f3xx.h"

void usart_gpio_init(gpio_pin_t* tx_pin, gpio_pin_t* rx_pin);

void usart_send_char_sync(USART_TypeDef *usart, char c);
char usart_recv_char_sync(USART_TypeDef *usart);

#endif // USART_H