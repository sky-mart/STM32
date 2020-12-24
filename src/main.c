/* 
 * This example shows how to use GPIO ports to get logical "1" or "0".
 * LED is attached to PE8. 
 */

#include "log.h"
#include "usart.h"
#include "lsm303dlhc.h"
#include "stm32f3xx.h"
#include "stdio.h"

void rcc_init()
{
	RCC->AHBENR |= 
		RCC_AHBENR_GPIOAEN | 
		RCC_AHBENR_GPIOBEN | 
		RCC_AHBENR_DMA1EN;
    RCC->APB1ENR |= 
    	RCC_APB1ENR_USART2EN |
    	RCC_APB1ENR_I2C1EN;
    RCC->CFGR3 |= RCC_CFGR3_USART2SW_SYSCLK; // set usart2 clock source to SYSCLK
}

void soft_sleep(uint32_t value)
{
   while (value > 0) {
           value--;
   }
}

void mag_print()
{
	log_printf_result_t log_result = LOG_PRINTF_SUCCESS;
	uint8_t mag_data[6];	

	lsm303dlhc_mag_regs_read(LSM303DLHC_OUT_X_H_M, mag_data, 6);
	log_result = log_printf(
		"X_H_M: %x\n"
		"X_L_M: %x\n"
		"Y_H_M: %x\n"
		"Y_L_M: %x\n"
		"Z_H_M: %x\n"
		"Z_L_M: %x\n\n", 
		mag_data[0],
		mag_data[1],
		mag_data[2],
		mag_data[3],
		mag_data[4],
		mag_data[5]
	);
}

int main()
{
	char c = 0;
	log_printf_result_t log_result = LOG_PRINTF_SUCCESS;

	SystemCoreClockUpdate();
   
    rcc_init();
    log_init();
    lsm303dlhc_init();

    mag_print();
    
    while (1) {
        c = usart_recv_char_sync(USART2); // usart2 is initialized in log_init
        log_result = log_printf("received char: %c\n", c);
        mag_print();	
    }
}

