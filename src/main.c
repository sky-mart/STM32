/*
 * This example shows how to use GPIO ports to get logical "1" or "0".
 * LED is attached to PE8.
 */

#include "log.h"
#include "lsm303dlhc.h"
#include "stdio.h"
#include "stm32f3xx.h"
#include "usart.h"

void rcc_init()
{
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_HSI;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_DMA1EN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
}

void soft_sleep(uint32_t value)
{
    while (value > 0) {
        value--;
    }
}

void lsm303dlhc_measure_and_print()
{
    log_printf_result_t log_result = LOG_PRINTF_SUCCESS;
    uint8_t data[7];

    // lsm303dlhc_mag_regs_read_sync(LSM303DLHC_OUT_X_H_M, data, 6);
    lsm303dlhc_mag_regs_read_async(LSM303DLHC_OUT_X_H_M, data, 6);
    lsm303dlhc_read_async_wait_to_finish();
    log_result = log_printf(
        "X_H_M: %X\n"
        "X_L_M: %X\n"
        "Y_H_M: %X\n"
        "Y_L_M: %X\n"
        "Z_H_M: %X\n"
        "Z_L_M: %X\n\n",
        data[0], data[1], data[2], data[3], data[4], data[5]);

    lsm303dlhc_acc_regs_read_async(LSM303DLHC_OUT_X_H_A, data, 7);
    lsm303dlhc_read_async_wait_to_finish();
    log_result = log_printf(
        "STATUS_REG_A: %X\n"
        "X_H_A: %X\n"
        "X_L_A: %X\n"
        "Y_H_A: %X\n"
        "Y_L_A: %X\n"
        "Z_H_A: %X\n"
        "Z_L_A: %X\n\n",
        data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
}

int main()
{
    char c                         = 0;
    log_printf_result_t log_result = LOG_PRINTF_SUCCESS;

    SystemCoreClockUpdate();

    rcc_init();
    log_init();
    lsm303dlhc_init();

    lsm303dlhc_measure_and_print();

    while (1) {
        c = usart_recv_char_sync(USART1);  // usart1 is initialized in log_init
        log_result = log_printf("received char: %c\n", c);
        lsm303dlhc_measure_and_print();
    }
}
