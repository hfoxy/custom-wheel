//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"

#include "button_input.h"

#define BUTTON_COUNT 23

uint8_t button_config[BUTTON_COUNT] = {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
};

void button_input_init(void)
{
    /* Configure all the buttons as inputs.*/
    for(int i = 0; i < BUTTON_COUNT; i++)
    {
        gpio_init(button_config[i]);
        gpio_set_dir(button_config[i], GPIO_IN);
    }
}

uint8_t button_input_read(uint8_t button_index)
{
    return (uint8_t)(gpio_get(button_config[button_index]) != 0);
}

uint32_t button_input_read_all(void)
{
    uint32_t ret_val = 0;
    for(int i = 0; i < BUTTON_COUNT; i++)
    {
        ret_val = button_input_read(i) << i;
    }
    return ret_val;
}