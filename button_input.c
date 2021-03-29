//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"

#include "button_input.h"
#include "button_assignments.h"

#define BUTTON_COUNT 15

button_t button_temp[BUTTON_COUNT] = {
        [0].gpio_num = 0,
        [0].bit_index = button_0,

        [1].gpio_num = 1,
        [1].bit_index = button_1,

        [2].gpio_num = 2,
        [2].bit_index = button_2,

        [3].gpio_num = 3,
        [3].bit_index = button_3,

        [4].gpio_num = 4,
        [4].bit_index = button_4,

        [5].gpio_num = 5,
        [5].bit_index = button_5,

        [6].gpio_num = 6,
        [6].bit_index = button_6,

        [7].gpio_num = 7,
        [7].bit_index = button_7,

        [8].gpio_num = 8,
        [8].bit_index = momentary_0_A,

        [9].gpio_num = 9,
        [9].bit_index = momentary_0_B,

        [10].gpio_num = 10,
        [10].bit_index = momentary_1_A,

        [11].gpio_num = 11,
        [11].bit_index = momentary_1_B,

        [12].gpio_num = 12,
        [12].bit_index = rotary_button_0,

        [13].gpio_num = 13,
        [13].bit_index = rotary_button_1,

        [14].gpio_num = 13,
        [14].bit_index = rotary_button_2,
};

void button_input_init(void)
{
    /* Configure all the buttons as inputs.*/
    for(int i = 0; i < BUTTON_COUNT; i++)
    {
        gpio_init(button_temp[i].gpio_num);
        gpio_set_dir(button_temp[i].gpio_num, GPIO_IN);
        gpio_pull_up(button_temp[i].gpio_num);
    }
}

uint8_t button_input_read(uint8_t bit_index)
{
    uint8_t found = 0;
    uint8_t i = 0;
    /* Try and locate the specific button related to the bit index passed in. */
    while((found == 0) && (i < BUTTON_COUNT))
    {
        if(button_temp[i].bit_index == bit_index)
        {
            found = 1;
        }
        else
        {
            i++;
        }
    }

    if(i > BUTTON_COUNT)
    {
        /* Request for pin was full of shit. and broke this. */
        //__breakpoint();
    }

    return (uint8_t)(gpio_get(button_temp[i].gpio_num) != 0);
}

uint32_t button_input_read_all(void)
{
    uint32_t ret_val = 0;
    for(int i = 0; i < BUTTON_COUNT; i++)
    {
        ret_val |= button_input_read(i) << button_temp[i].bit_index;
    }
    return ret_val;
}