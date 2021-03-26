//
// Created by p209287 on 26/03/2021.
//

#ifndef CUSTOM_WHEEL_BUTTON_INPUT_H
#define CUSTOM_WHEEL_BUTTON_INPUT_H

void button_input_init(void);
uint8_t button_input_read(uint8_t button_index);
uint32_t button_input_read_all(void);

#endif //CUSTOM_WHEEL_BUTTON_INPUT_H
