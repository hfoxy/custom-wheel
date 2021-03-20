
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "io-expander.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define I2C_addr 0x18

#define PIOCON0 0x9E
#define PIOCON1 0xC9


typedef struct {
    uint8_t port;
    uint8_t pin;
}pin_identification_t;

typedef struct {
    pin_identification_t pin_identification[14];
}pin_identification_array_t;

pin_identification_array_t pin_identification_array  = {
        .pin_identification[0].pin  = 5,
        .pin_identification[0].port = 1,

        .pin_identification[1].pin  = 0,
        .pin_identification[1].port = 1,

        .pin_identification[2].pin  = 2,
        .pin_identification[2].port = 1,

        .pin_identification[3].pin  = 4,
        .pin_identification[3].port = 1,

        .pin_identification[4].pin  = 0,
        .pin_identification[4].port = 0,

        .pin_identification[5].pin  = 1,
        .pin_identification[5].port = 0,

        .pin_identification[6].pin  = 1,
        .pin_identification[6].port = 1,

        .pin_identification[7].pin  = 3,
        .pin_identification[7].port = 0,

        .pin_identification[8].pin  = 4,
        .pin_identification[8].port = 0,

        .pin_identification[9].pin  = 0,
        .pin_identification[9].port = 3,

        .pin_identification[10].pin  = 6,
        .pin_identification[10].port = 0,

        .pin_identification[11].pin  = 5,
        .pin_identification[11].port = 0,

        .pin_identification[12].pin  = 7,
        .pin_identification[12].port = 0,

        .pin_identification[13].pin  = 7,
        .pin_identification[13].port = 1,
};

uint8_t pm1_translator[4] = {0x71, 0x72, 0x00, 0x6C};
uint8_t px_translator[4] = {0x40, 0x50, 0x60, 0x70};
uint8_t pxs_translator[4] = {0xC2, 0xC4, 0x75, 0xC0};

void read8(uint8_t reg_address, uint8_t *dest);
void write8(uint8_t reg_address, uint8_t *source);
void set_bit(uint8_t reg_address, uint8_t bit_index);
void clr_bit(uint8_t reg_address, uint8_t bit_index);

void read8(uint8_t reg_address, uint8_t *dest)
{
    if (i2c_write_blocking(I2C_PORT, I2C_addr, &reg_address, 1,true) < PICO_ERROR_NONE)
    {
        __breakpoint();
    }

    if (i2c_read_blocking(I2C_PORT, I2C_addr, dest, 1, false) < PICO_ERROR_NONE)
    {
        __breakpoint();
    }
}

void write8(uint8_t reg_address, uint8_t *source)
{
    if (i2c_write_blocking(I2C_PORT, I2C_addr, &reg_address, 1,true) < PICO_ERROR_NONE)
    {
        __breakpoint();
    }
    if (i2c_write_blocking(I2C_PORT, I2C_addr, source, 1, false) < PICO_ERROR_NONE)
    {
        __breakpoint();
    }
}

void set_bit(uint8_t reg_address, uint8_t bit_index)
{
    uint8_t reg_value;
    read8(reg_address, &reg_value);

    reg_value |= (1 << bit_index);

    write8(reg_address, &reg_value);
}

void clr_bit(uint8_t reg_address, uint8_t bit_index)
{
    uint8_t reg_value;
    read8(reg_address, &reg_value);

    reg_value &= ~(1 << bit_index);

    write8(reg_address, &reg_value);
}

void set_mode(uint8_t pin)
{
    uint8_t array_pin = pin - 1;

    uint8_t pm1;
    read8(pm1_translator[pin_identification_array.pin_identification[array_pin].port], &pm1);

    uint8_t pm2;
    read8(pm1_translator[pin_identification_array.pin_identification[array_pin].port] + 1, &pm2);


    /* Clear as for input PM1 and PM2 should be 0. */
    pm1 |= (1 << pin_identification_array.pin_identification[array_pin].pin);
    pm2 &= ~(1 << pin_identification_array.pin_identification[array_pin].pin);

    /* Write these pieces of shit. */
    write8(pm1_translator[pin_identification_array.pin_identification[array_pin].port] , &pm1);

    write8(pm1_translator[pin_identification_array.pin_identification[array_pin].port] + 1, &pm2);

    /* Enable schmitt trigger. */
    set_bit(pxs_translator[pin_identification_array.pin_identification[array_pin].port], pxs_translator[pin_identification_array.pin_identification[array_pin].pin]);

    /* Set initial Pin state/ Pull up/down. */
    //uint8_t temp = (uint8_t)(0b1000 | px_translator[pin_identification_array.pin_identification[array_pin].pin]);
    //write8(px_translator[pin_identification_array.pin_identification[array_pin].port], &temp);
}

void ioExpander_init(void)
{
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);

    // Pin setups
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    /* Disable PWM all round. */
    uint8_t clear_reg = 0x00;
    write8(PIOCON0, &clear_reg);
    write8(PIOCON1, &clear_reg);

    // IO Expander configuration.
    set_mode(14);

}

uint8_t get_value(uint8_t pin)
{
    uint8_t array_pin = pin - 1;
    uint8_t data;
    read8(px_translator[pin_identification_array.pin_identification[array_pin].port] , &data);
    return (data & (1 << pin_identification_array.pin_identification[array_pin].pin)) >> pin_identification_array.pin_identification[array_pin].pin;
}
