//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

#include "rotary_input.h"
#include "button_assignments.h"

#define ROTARY_ENCODER_COUNT 3U
#define ROTARY_DEBOUNCE_ALARM 0U
#define ROTARY_DEBOUNCE_COUNT 10U
#define ROTARY_DEBOUNCE_TIMEOUT 100U

static const rotary_t rotary_encoders[ROTARY_ENCODER_COUNT] = {
    [0].cw_bit_index = rotary_0_cw,
    [0].ccw_bit_index = rotary_0_ccw,
    [0].A_gpio_num = 16,
    [0].B_gpio_num = 18,

    [1].cw_bit_index = rotary_1_cw,
    [1].ccw_bit_index = rotary_1_ccw,
    [1].A_gpio_num = 17,
    [1].B_gpio_num = 18,

    [2].cw_bit_index = rotary_2_cw,
    [2].ccw_bit_index = rotary_2_ccw,
    [2].A_gpio_num = 19,
    [2].B_gpio_num = 20,
};

typedef struct{
    uint8_t A;
    uint8_t B;
}channel_data_t;

typedef struct{
    channel_data_t channel_data[ROTARY_DEBOUNCE_COUNT];
}encoder_rotary_data_t;

encoder_rotary_data_t rotary_array[ROTARY_ENCODER_COUNT];

int8_t rotary_data[ROTARY_ENCODER_COUNT];

void rotary_encoder_capture_value(void)
{
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        for(int j = 0; j < (ROTARY_DEBOUNCE_COUNT - 1); i++)
        {
            rotary_array[i].channel_data[j].A = rotary_array[i].channel_data[j + 1].A;
            rotary_array[i].channel_data[j].B = rotary_array[i].channel_data[j + 1].B;
        }
        rotary_array[i].channel_data[ROTARY_DEBOUNCE_COUNT].A = gpio_get(rotary_encoders[i].A_gpio_num);
        rotary_array[i].channel_data[ROTARY_DEBOUNCE_COUNT].B = gpio_get(rotary_encoders[i].B_gpio_num);
    }
}

void rotary_encoder_irq_handler()
{
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        hardware_alarm_claim(ROTARY_DEBOUNCE_ALARM);
        /* For each rotary encoder we have connected. */
        if(gpio == rotary_encoders[i].A_gpio_num)
        {
            /* If the interrupt occurred on this rotary encoder. */
            uint8_t B = gpio_get(rotary_encoders[i].B_gpio_num);

            if(((events & GPIO_IRQ_EDGE_FALL) != 0)
            && (events & GPIO_IRQ_EDGE_RISE) == 0)
            {
                /* if we are seeing a falling edge on A. */
                if(B == 0)
                {
                    /* B is low so Counter Clock Wise. */
                    rotary_data[i]--;
                }
                else
                {
                    /* B is high so Clock Wise. */
                    rotary_data[i]++;
                }
            }
            else if ((events & GPIO_IRQ_EDGE_RISE) != 0)
            {
                /* if we are seeing a rising edge on A. */
                if(B == 0)
                {
                    /* B is low so Clock Wise. */
                    rotary_data[i]++;
                }
                else
                {
                    /* B is high so Counter Clock Wise. */
                    rotary_data[i]--;
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

repeating_timer_t debounce_timer;

void rotary_input_init(void)
{
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        gpio_init(rotary_encoders[i].A_gpio_num);
        gpio_set_dir(rotary_encoders[i].A_gpio_num, GPIO_IN);

        gpio_init(rotary_encoders[i].B_gpio_num);
        gpio_set_dir(rotary_encoders[i].B_gpio_num, GPIO_IN);
        /*gpio_set_irq_enabled_with_callback(rotary_encoders[i].A_gpio_num,
                             GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE,
                             true,
                             &rotary_encoder_irq_handler);*/
    }

    /* setup some more stuff. */
    add_repeating_timer_us(ROTARY_DEBOUNCE_TIMEOUT,
                           rotary_encoder_capture_value,
                           0,
                           &debounce_timer);
    for(int i = 0; i < ROTARY_ENCODER_COUNT; i++)
    {
        for(int j = 0; j < ROTARY_DEBOUNCE_COUNT; j++)
        {
            rotary_array[i].channel_data[j].A = gpio_get(rotary_encoders[i].A_gpio_num);
            rotary_array[i].channel_data[j].B = gpio_get(rotary_encoders[i].B_gpio_num);
        }
    }

}

