/* 
 * This example shows how to use GPIO ports to get logical "1" or "0".
 * LED is attached to PE8. 
 */

#include "stm32f3xx.h"

GPIO_TypeDef *port = GPIOE;
int led = 8;

volatile void softDelay(uint32_t time)
{
	while (time > 0)
	{
		time--;
	}
}

int main() {
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN; // Turn tacting GPIOE on
	
	port->MODER 	|= 1 << led*2; 	// General purpose output mode
	port->OTYPER 	&= ~(1 << led); // Push-pull
	port->OSPEEDR 	|= 3 << led*2;	// High speed
	port->PUPDR 	|= 1 << led*2;	// Pull-up
	
	while (1) {
		port->BSRR |= 1 << led;			// Output "1"
		softDelay(400000);
		port->BSRR |= 1 << (led + 16);	// Output "0"
		softDelay(400000);
	}
}
