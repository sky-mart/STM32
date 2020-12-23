#include "lsm303dlhc.h"
#include "i2c.h"
#include "gpio.h"
#include "stm32f3xx.h"

typedef struct {
	I2C_TypeDef* i2c;
	gpio_pin_t scl_pin;
	gpio_pin_t sda_pin;
} lsm303dlhc_t;

static lsm303dlhc_t lsm = {
	I2C1, // i2c
	{GPIOB, 6, 4}, // scl_pin PB6, AF4
	{GPIOB, 7, 4}, // sda_pin PB7, AF4
};

typedef enum {
	LSM303DLHC_ACCELEROMETER = 0x19,
	LSM303DLHC_MAGNETOMETER = 0x1e
} lsm303dlhc_i2c_address_t;

static void lsm303dlhc_i2c_init()
{
	i2c_gpio_init(&lsm.scl_pin, &lsm.sda_pin);
    
    lsm.i2c->CR1 = 0;
    lsm.i2c->TIMINGR = 
    	(0 << I2C_TIMINGR_PRESC_Pos) |		// i2c frequency is 8MHz
    	(1 << I2C_TIMINGR_SDADEL_Pos) |
    	(0 << I2C_TIMINGR_SCLDEL_Pos) |
    	(10 << I2C_TIMINGR_SCLL_Pos) |		// T_low = 1375 ns
    	(4 << I2C_TIMINGR_SCLH_Pos);		// T_high = 625 ns
    lsm.i2c->CR1 |= I2C_CR1_PE;
}

void lsm303dlhc_init()
{
	lsm303dlhc_i2c_init();
}

static uint8_t lsm303dlhc_reg_read(uint8_t slave, uint8_t reg)
{
	lsm.i2c->CR2 &= ~I2C_CR2_SADD;		
	lsm.i2c->CR2 |= slave << 1;
        
    lsm.i2c->CR2 &= ~(
    	I2C_CR2_RD_WRN |	// write
    	I2C_CR2_AUTOEND		// no auto stop
    );  
    lsm.i2c->CR2 &= ~I2C_CR2_NBYTES;
    lsm.i2c->CR2 |= 1 << I2C_CR2_NBYTES_Pos;	// 1 byte
    lsm.i2c->CR2 |= I2C_CR2_START;
    
    while (!(lsm.i2c->ISR & I2C_ISR_TXIS)) {
    	__NOP(); 
    }
    lsm.i2c->TXDR = reg;
    
    while (!(lsm.i2c->ISR & I2C_ISR_TC)) {
    	__NOP();
    }

	lsm.i2c->CR2 |= 
		I2C_CR2_RD_WRN |	// read
		I2C_CR2_AUTOEND;    // auto stop
    lsm.i2c->CR2 |= 1 << I2C_CR2_NBYTES_Pos;	// 1 byte
    lsm.i2c->CR2 |= I2C_CR2_START;
    
    while (!(lsm.i2c->ISR & I2C_ISR_RXNE)) {
    	__NOP();
    }
    return lsm.i2c->RXDR;
}

uint8_t lsm303dlhc_acc_reg_read(lsm303dlhc_acc_reg_t reg)
{
	return lsm303dlhc_reg_read(LSM303DLHC_ACCELEROMETER, reg);
}

uint8_t lsm303dlhc_mag_reg_read(lsm303dlhc_mag_reg_t reg)
{
	return lsm303dlhc_reg_read(LSM303DLHC_MAGNETOMETER, reg);
}