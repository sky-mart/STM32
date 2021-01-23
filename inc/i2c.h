#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#include "gpio.h"

void i2c_gpio_init(gpio_pin_t* scl_pin, gpio_pin_t* sda_pin);

#endif  // I2C_H