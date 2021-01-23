#include "gpio.h"

#define GPIO_MODE_MASK            0x3
#define GPIO_MODE_SHIFT_PER_PIN   2
#define GPIO_OTYPE_MASK           0x1
#define GPIO_OTYPE_SHIFT_PER_PIN  1
#define GPIO_OSPEED_MASK          0x3
#define GPIO_OSPEED_SHIFT_PER_PIN 2
#define GPIO_PUPD_MASK            0x3
#define GPIO_PUPD_SHIFT_PER_PIN   2

void gpio_pin_set_mode(gpio_pin_t* gpio_pin, gpio_mode_t value)
{
    gpio_pin->gpio->MODER &=
        ~(GPIO_MODE_MASK << (gpio_pin->pin * GPIO_MODE_SHIFT_PER_PIN));
    gpio_pin->gpio->MODER |= value << (gpio_pin->pin * GPIO_MODE_SHIFT_PER_PIN);
}

void gpio_pin_set_otype(gpio_pin_t* gpio_pin, gpio_otype_t value)
{
    gpio_pin->gpio->OTYPER &=
        ~(GPIO_OTYPE_MASK << (gpio_pin->pin * GPIO_OTYPE_SHIFT_PER_PIN));
    gpio_pin->gpio->OTYPER |= value
                              << (gpio_pin->pin * GPIO_OTYPE_SHIFT_PER_PIN);
}

void gpio_pin_set_ospeed(gpio_pin_t* gpio_pin, gpio_ospeed_t value)
{
    gpio_pin->gpio->OSPEEDR &=
        ~(GPIO_OSPEED_MASK << (gpio_pin->pin * GPIO_OSPEED_SHIFT_PER_PIN));
    gpio_pin->gpio->OSPEEDR |= value
                               << (gpio_pin->pin * GPIO_OSPEED_SHIFT_PER_PIN);
}

void gpio_pin_set_pupd(gpio_pin_t* gpio_pin, gpio_pupd_t value)
{
    gpio_pin->gpio->PUPDR &=
        ~(GPIO_PUPD_MASK << (gpio_pin->pin * GPIO_PUPD_SHIFT_PER_PIN));
    gpio_pin->gpio->PUPDR |= value << (gpio_pin->pin * GPIO_PUPD_SHIFT_PER_PIN);
}

void gpio_pin_set(gpio_pin_t* gpio_pin)
{
    gpio_pin->gpio->BSRR |= 1 << gpio_pin->pin;
}

void gpio_pin_reset(gpio_pin_t* gpio_pin)
{
    gpio_pin->gpio->BSRR |= 1 << (gpio_pin->pin + 16);
}

void gpio_pin_set_altfun(gpio_pin_t* gpio_pin)
{
    gpio_pin->gpio->AFR[gpio_pin->pin >> 3] |= gpio_pin->altfun
                                               << ((gpio_pin->pin & 7) * 4);
}
