#include "i2c.h"

void i2c_gpio_init(gpio_pin_t* scl_pin, gpio_pin_t* sda_pin)
{
    gpio_pin_set_mode(scl_pin, GPIO_MODE_ALTER);
    gpio_pin_set_otype(scl_pin, GPIO_OTYPE_OPENDRAIN);
    gpio_pin_set_ospeed(scl_pin, GPIO_OSPEED_HIGH);
    gpio_pin_set_pupd(scl_pin, GPIO_PUPD_PULLUP);
    gpio_pin_set_altfun(scl_pin);

    gpio_pin_set_mode(sda_pin, GPIO_MODE_ALTER);
    gpio_pin_set_otype(sda_pin, GPIO_OTYPE_OPENDRAIN);
    gpio_pin_set_ospeed(sda_pin, GPIO_OSPEED_HIGH);
    gpio_pin_set_pupd(sda_pin, GPIO_PUPD_PULLUP);
    gpio_pin_set_altfun(sda_pin);
}
