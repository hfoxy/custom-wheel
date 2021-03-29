//
// Created by p209287 on 26/03/2021.
//

#ifndef CUSTOM_WHEEL_ROTARY_INPUT_H
#define CUSTOM_WHEEL_ROTARY_INPUT_H

typedef struct {
    uint8_t cw_bit_index;
    uint8_t ccw_bit_index;
    uint8_t A_gpio_num;
    uint8_t B_gpio_num;
}rotary_t;

uint8_t rotary_input_check(void);
void rotary_input_read(int8_t *buffer);
void rotary_input_init(void);


#endif //CUSTOM_WHEEL_ROTARY_INPUT_H
