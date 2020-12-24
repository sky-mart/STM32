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

static void lsm303dlhc_prepare_reg_read(uint8_t slave, uint8_t reg)
{
	lsm.i2c->CR2 &= ~I2C_CR2_SADD;		
	lsm.i2c->CR2 |= slave << 1;	// have to shift by 1 because of 7-bit addr
        
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
}

static void lsm303dlhc_regs_read_sync(uint8_t slave, uint8_t begin_reg, uint8_t* data, uint8_t size)
{
	uint8_t i;
	lsm303dlhc_prepare_reg_read(slave, begin_reg);

	lsm.i2c->CR2 |= 
		I2C_CR2_RD_WRN |	// read
		I2C_CR2_AUTOEND;    // auto stop
    lsm.i2c->CR2 &= ~I2C_CR2_NBYTES;
    lsm.i2c->CR2 |= size << I2C_CR2_NBYTES_Pos;	// size bytes
    lsm.i2c->CR2 |= I2C_CR2_START;
    
    for (i = 0; i < size; i++) {
    	while (!(lsm.i2c->ISR & I2C_ISR_RXNE)) {
    		__NOP();
    	}
    	data[i] = lsm.i2c->RXDR;
    }
}

uint8_t lsm303dlhc_acc_reg_read_sync(lsm303dlhc_acc_reg_t reg)
{
    uint8_t reg_value;
    lsm303dlhc_regs_read_sync(LSM303DLHC_ACCELEROMETER, reg, &reg_value, 1);
    return reg_value;
}

uint8_t lsm303dlhc_mag_reg_read_sync(lsm303dlhc_mag_reg_t reg)
{
    uint8_t reg_value;
    lsm303dlhc_regs_read_sync(LSM303DLHC_MAGNETOMETER, reg, &reg_value, 1);
    return reg_value;
}

void lsm303dlhc_acc_regs_read_sync(lsm303dlhc_acc_reg_t reg, uint8_t* data, uint8_t size)
{
	return lsm303dlhc_regs_read_sync(LSM303DLHC_ACCELEROMETER, reg, data, size);
}

void lsm303dlhc_mag_regs_read_sync(lsm303dlhc_mag_reg_t reg, uint8_t* data, uint8_t size)
{
	return lsm303dlhc_regs_read_sync(LSM303DLHC_MAGNETOMETER, reg, data, size);
}