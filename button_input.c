//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"

#include "button_input.h"

#define BUTTON_COUNT 15

uint8_t button_config[BUTTON_COUNT] = {
        0,  /* button_0 */
        1,  /* button_1 */
        2,  /* button_2 */
        3,  /* button_3 */
        4,  /* button_4 */
        5,  /* button_5 */
        6,  /* button_6 */
        7,  /* button_7 */
        8,  /* momentary_0_A */
        9,  /* momentary_0_B */
        10, /* momentary_1_A */
        11, /* momentary_1_B */
        12, /* rotary_button_0 */
        13, /* rotary_button_1 */
        14, /* rotary_button_2 */
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