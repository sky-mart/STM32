#ifndef GPIO_H
#define GPIO_H

#include "stm32f3xx.h"
#include "typedefs.h"

typedef enum {
    GPIO_MODE_INPUT  = 0x0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_ALTER  = 0x2,
    GPIO_MODE_ANALOG = 0x3
} gpio_mode_t;

typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x0,
    GPIO_OTYPE_OPENDRAIN = 0x1
} gpio_otype_t;

typedef enum {
    GPIO_OSPEED_LOW    = 0x0,
    GPIO_OSPEED_MEDIUM = 0x1,
    GPIO_OSPEED_HIGH   = 0x2
} gpio_ospeed_t;

typedef enum {
    GPIO_PUPD_NOPULL   = 0x0,
    GPIO_PUPD_PULLUP   = 0x1,
    GPIO_PUPD_PULLDOWN = 0x2
} gpio_pupd_t;

typedef struct {
    GPIO_TypeDef* gpio;
    u8 pin;
    u8 altfun;
} gpio_pin_t;

void gpio_pin_set_mode(gpio_pin_t* gpio_pin, gpio_mode_t value);
void gpio_pin_set_otype(gpio_pin_t* gpio_pin, gpio_otype_t value);
void gpio_pin_set_ospeed(gpio_pin_t* gpio_pin, gpio_ospeed_t value);
void gpio_pin_set_pupd(gpio_pin_t* gpio_pin, gpio_pupd_t value);
void gpio_pin_set(gpio_pin_t* gpio_pin);
void gpio_pin_reset(gpio_pin_t* gpio_pin);
void gpio_pin_set_altfun(gpio_pin_t* gpio_pin);

#endif  // GPIO_H