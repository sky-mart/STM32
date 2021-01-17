#include "lsm303dlhc.h"
#include "i2c.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f3xx.h"

typedef struct {
	I2C_TypeDef* i2c;
	gpio_pin_t scl_pin;
	gpio_pin_t sda_pin;
    DMA_Channel_TypeDef* dma_channel;
    IRQn_Type dma_channel_irq;
} lsm303dlhc_t;

static lsm303dlhc_t lsm = {
	I2C1, // i2c
	{GPIOB, 6, 4}, // scl_pin PB6, AF4
	{GPIOB, 7, 4}, // sda_pin PB7, AF4
    DMA1_Channel7, // dma_channel
    DMA1_Channel7_IRQn
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

// sub is an address in LSM303DLHC
static void lsm303dlhc_write_sub(uint8_t slave, uint8_t sub)
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
    lsm.i2c->TXDR = sub;
    
    while (!(lsm.i2c->ISR & I2C_ISR_TC)) {
    	__NOP();
    }
}

static void lsm303dlhc_regs_read_sync(uint8_t slave, uint8_t begin_reg, uint8_t* data, uint8_t size)
{
	uint8_t i;
	lsm303dlhc_write_sub(slave, (1 << 7) | begin_reg);

    lsm.i2c->CR1 &= ~I2C_CR1_RXDMAEN;
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

static void lsm303dlhc_regs_read_async(uint8_t slave, uint8_t begin_reg, uint8_t* data, uint8_t size)
{
    dma_transfer_t dma_trasfer;
    dma_trasfer.memory_addr = (void*)data;
    dma_trasfer.periph_addr = (void*)&lsm.i2c->RXDR;
    dma_trasfer.count = size;
    *(uint32_t*)&dma_trasfer.config = 0U;
    // dma_trasfer.config.TCIE = dma_irq;
    dma_trasfer.config.MINC = 1;

    dma_channel_init(lsm.dma_channel, &dma_trasfer);
    lsm.i2c->CR1 |= I2C_CR1_RXDMAEN;

    lsm303dlhc_write_sub(slave, (1 << 7) | begin_reg);

    lsm.i2c->CR2 |= 
        I2C_CR2_RD_WRN |    // read
        I2C_CR2_AUTOEND;    // auto stop
    lsm.i2c->CR2 &= ~I2C_CR2_NBYTES;
    lsm.i2c->CR2 |= size << I2C_CR2_NBYTES_Pos; // size bytes
    lsm.i2c->CR2 |= I2C_CR2_START;
}

static void lsm303dlhc_reg_write_sync(uint8_t slave, uint8_t reg, uint8_t value)
{
    lsm.i2c->CR2 &= ~I2C_CR2_SADD;      
    lsm.i2c->CR2 |= slave << 1; // have to shift by 1 because of 7-bit addr
        
    lsm.i2c->CR2 &= ~(
        I2C_CR2_RD_WRN |            // write
        I2C_CR2_NBYTES              // clear nbytes
    );  
    lsm.i2c->CR2 |= 
        I2C_CR2_AUTOEND |           // auto stop
        (2 << I2C_CR2_NBYTES_Pos);  // 2 bytes
    lsm.i2c->CR2 |= I2C_CR2_START;
    
    while (!(lsm.i2c->ISR & I2C_ISR_TXIS)) {
        __NOP(); 
    }
    lsm.i2c->TXDR = reg;

    while (!(lsm.i2c->ISR & I2C_ISR_TXIS)) {
        __NOP(); 
    }
    lsm.i2c->TXDR = value;
}

void lsm303dlhc_read_async_wait_to_finish()
{
    while (!(DMA1->ISR & DMA_ISR_TCIF7)) {
        __NOP();
    }
    DMA1->IFCR |= DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7;
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

void lsm303dlhc_acc_regs_read_async(lsm303dlhc_acc_reg_t reg, uint8_t* data, uint8_t size)
{
    return lsm303dlhc_regs_read_async(LSM303DLHC_ACCELEROMETER, reg, data, size);
}

void lsm303dlhc_mag_regs_read_async(lsm303dlhc_mag_reg_t reg, uint8_t* data, uint8_t size)
{
    return lsm303dlhc_regs_read_async(LSM303DLHC_MAGNETOMETER, reg, data, size);
}

void lsm303dlhc_acc_reg_write_sync(lsm303dlhc_acc_reg_t reg, uint8_t value)
{
    lsm303dlhc_reg_write_sync(LSM303DLHC_ACCELEROMETER, reg, value);
}

void lsm303dlhc_mag_reg_write_sync(lsm303dlhc_mag_reg_t reg, uint8_t value)
{
    lsm303dlhc_reg_write_sync(LSM303DLHC_MAGNETOMETER, reg, value);
}

static void lsm303dlhc_mag_init()
{
    uint8_t check; // debug variable

    // 0.75 Hz, temperature sensor off
    lsm303dlhc_mag_reg_write_sync(LSM303DLHC_CRA_REG_M, 0);
    check = lsm303dlhc_mag_reg_read_sync(LSM303DLHC_CRA_REG_M);

    // range +- 1.3 Gauss
    lsm303dlhc_mag_reg_write_sync(LSM303DLHC_CRB_REG_M, 0x20);
    check = lsm303dlhc_mag_reg_read_sync(LSM303DLHC_CRB_REG_M);

    // continuous conversion mode
    lsm303dlhc_mag_reg_write_sync(LSM303DLHC_MR_REG_M, 0);
    check = lsm303dlhc_mag_reg_read_sync(LSM303DLHC_MR_REG_M);
}

static void lsm303dlhc_acc_init()
{
    uint8_t check; // debug variable

    // 1 Hz, 3 axis
    lsm303dlhc_acc_reg_write_sync(LSM303DLHC_CTRL_REG1_A, 0x17);
    // check = lsm303dlhc_acc_reg_read_sync(LSM303DLHC_CTRL_REG1_A);

    // +- 16G, high resolution
    lsm303dlhc_acc_reg_write_sync(LSM303DLHC_CTRL_REG4_A, 0x38);
    // check = lsm303dlhc_acc_reg_read_sync(LSM303DLHC_CTRL_REG4_A);

    // data ready interrupt on INT1
    // lsm303dlhc_acc_reg_write_sync(LSM303DLHC_CTRL_REG3_A, 0x10);
    // check = lsm303dlhc_acc_reg_read_sync(LSM303DLHC_CTRL_REG3_A);
}

void lsm303dlhc_init()
{
    lsm303dlhc_i2c_init();
    lsm303dlhc_acc_init();    
    lsm303dlhc_mag_init();
}