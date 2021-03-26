//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"
#include "hardware/irq.h"


#include "rotary_input.h"
#include "button_assignments.h"

#define ROTARY_ENCODER_COUNT 3U

typedef struct{
    uint8_t A;
    uint8_t B;
}rotary_encoder_t;

static const rotary_encoder_t rotary_encoder_pins[ROTARY_ENCODER_COUNT] = {
    [0].A = 1,
    [0].B = 2,

    [1].A = 3,
    [1].B = 4,

    [2].A = 5,
    [2].B = 6,
};

static const rotary_encoder_t rotary_encoder_buttons[ROTARY_ENCODER_COUNT] = {
        [0].A = rotary_0_cw,
        [0].B = rotary_0_ccw,

        [1].A = rotary_1_cw,
        [1].B = rotary_1_ccw,

        [2].A = rotary_2_cw,
        [2].B = rotary_2_ccw,
};

int8_t rotary_data[ROTARY_ENCODER_COUNT];


void rotary_encoder_irq_handler(uint gpio, uint32_t events)
{
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        /* For each rotary encoder we have connected. */
        if(gpio == rotary_encoder_pins[i].A)
        {
            /* If the interrupt occurred on this rotary encoder. */
            uint8_t B = gpio_get(rotary_encoder_pins[i].B);

            if((events & GPIO_IRQ_EDGE_FALL) != 0)
            {
                /* if we are seeing a falling edge on A. */
                if(B == 0)
                {
                    /* B is low so Counter Clock Wise. */
                    rotary_data[i] = -1;
                }
                else
                {
                    /* B is high so Clock Wise. */
                    rotary_data[i] = 1;
                }
            }
            else if ((events & GPIO_IRQ_EDGE_RISE) != 0)
            {
                /* if we are seeing a rising edge on A. */
                if(B == 0)
                {
                    /* B is low so Clock Wise. */
                    rotary_data[i] = 1;
                }
                else
                {
                    /* B is high so Counter Clock Wise. */
                    rotary_data[i] = -1;
                }
            }
        }
    }
}

uint8_t rotary_input_check(void)
{
    uint8_t ret_val = 0;
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        if(rotary_data[i] != 0)
        {
            ret_val |= 1;
        }
    }
    return ret_val;
}

/**
 *
 * @param buffer - Needs to be an array at least 3 long.
 */
void rotary_input_read(int8_t *buffer)
{
    buffer = rotary_data;
}

void rotary_input_init(void)
{
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        gpio_init(rotary_encoder_pins[i].A);
        gpio_set_dir(rotary_encoder_pins[i].A, GPIO_IN);

        gpio_init(rotary_encoder_pins[i].B);
        gpio_set_dir(rotary_encoder_pins[i].B, GPIO_IN);
        gpio_set_irq_enabled_with_callback(rotary_encoder_pins[i].A,
                             GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                             true,
                             &rotary_encoder_irq_handler);
    }
}

