#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "pico/multicore.h"

#include "bsp/board.h"
#include "tusb.h"

#include "button_assignments.h"
#include "button_input.h"
#include "rotary_input.h"
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
/*#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19*/

#define PIN_CS   17



// GPIO defines
#define BTN_01 0
#define BTN_02 1
#define BTN_03 2
#define BTN_04 3
#define BTN_05 4
#define BTN_06 5
#define BTN_07 6
#define BTN_08 7

#define TOGGLE_01_A 8
#define TOGGLE_01_B 9

#define TOGGLE_02_A 16
#define TOGGLE_02_B 17

#define SHIFT_UP 18
#define SHIFT_DOWN 19

#define ROTARY_01_A 10
#define ROTARY_01_B 11
#define ROTARY_01_C 12

#define ROTARY_02_A 15
#define ROTARY_02_B 14
#define ROTARY_02_C 13

#define ROTARY_03_A 26
#define ROTARY_03_B 27
#define ROTARY_03_C 28

typedef struct {
    uint8_t buttons_a;
    uint8_t buttons_b;
    uint8_t buttons_c;
} gamepad_report_t;

typedef struct {
    uint8_t gpio_a;
    uint8_t gpio_b;
    uint8_t btn_idx;
    uint8_t bit_idx;
    uint8_t *report_btn;
} rotary_gpio_t;

static gamepad_report_t report;

uint8_t buttons[24];
uint8_t array[3];

rotary_gpio_t rotary_gpios[29];
uint8_t rotary_updated = 0;

void handle_rotary_state_change(uint gpio, uint32_t events);

int update_gpio(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx);

int update_gpio_with_pressed(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx, uint8_t pressed_value);

int update_ioe(int expander_io_num, uint8_t *btns, int btn_idx, int bit_idx);

int update_btn(uint8_t state, uint8_t pressed_value, uint8_t *btns, int btn_idx, int bit_idx);

void core1_entry();

void core1_entry()
{
    /* Chuck something so Core0 can continue. */
    multicore_fifo_push_blocking(0xFFFFFFFF);

    tusb_init();
    gamepad_report_t report_local;
    uint32_t data_received;
    while(1)
    {
        tud_task();
        if((multicore_fifo_get_status() & 0b0001) != 0)
        {
            multicore_fifo_pop_timeout_us(10000, &data_received);
            report_local.buttons_a = (uint8_t)(data_received & 0xFF);
            report_local.buttons_b = (uint8_t)((data_received >> 8) & 0xFF);
            report_local.buttons_c = (uint8_t)((data_received >> 16) & 0xFF);
            if (tud_hid_ready()) {
                tud_hid_report(0, &report_local, sizeof(report));
            }
        }
    }
}


int main() {
    stdio_init_all();

    multicore_launch_core1(core1_entry);

    multicore_fifo_pop_blocking();

    /*// SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);*/

    // Chip select is active-low, so we'll initialise it to a driven-high state
    //gpio_set_dir(PIN_CS, GPIO_OUT);
    //gpio_put(PIN_CS, 1);

    //ioExpander_init();

    //for (int i = 0; i < 10000; i++) {
    //    uint8_t ok = 10;
    //    sleep_ms(ok);
    //}

    board_init();
    button_input_init();
    rotary_input_init();

    uint32_t last_button_state;
    uint32_t button_state = 0xFF;
    uint64_t time_since = time_us_64();

    while (1)
    {
        int8_t rotary_data[3];

        if(rotary_input_check() != 0)
        {
            button_state = 0;
            rotary_input_read(rotary_data);
            if(rotary_data[0] > 0)
            {
                button_state |= ( 1 << rotary_0_cw);
            }
            else if (rotary_data[0] < 0)
            {
                button_state |= ( 1 << rotary_0_ccw);
            }
            if(rotary_data[1] > 0)
            {
                button_state |= ( 1 << rotary_1_cw);
            }
            else if (rotary_data[1] < 0)
            {
                button_state |= ( 1 << rotary_1_ccw);
            }
            if(rotary_data[2] > 0)
            {
                button_state |= ( 1 << rotary_2_cw);
            }
            else if (rotary_data[2] < 0)
            {
                button_state |= ( 1 << rotary_2_ccw);
            }
        }

        /* Read the state of the buttons. */
        //button_state |= button_input_read_all();

        if(last_button_state != button_state)
        {
            multicore_fifo_push_timeout_us(button_state,10000);
        }
        last_button_state = button_state;
    }

    return 0;
}

uint8_t last_a = 1;
uint8_t last_b = 1;
void handle_rotary_state_change(uint gpio, uint32_t events) {
    rotary_gpio_t rotary_gpio = rotary_gpios[gpio];

    rotary_gpio_t rotary_gpio_a = rotary_gpios[rotary_gpio.gpio_a];
    rotary_gpio_t rotary_gpio_b = rotary_gpios[rotary_gpio.gpio_b];

    uint8_t a = gpio_get(rotary_gpio.gpio_a);
    uint8_t b = gpio_get(rotary_gpio.gpio_b);
    if (a == 0 && b == 0 && last_b == 1) {
        rotary_updated += update_btn(0, 1, rotary_gpio_a.report_btn, rotary_gpio_a.btn_idx, rotary_gpio_a.bit_idx);
        rotary_updated += update_btn(1, 1, rotary_gpio_b.report_btn, rotary_gpio_b.btn_idx, rotary_gpio_b.bit_idx);
    } else if (b == 0 && a == 0 && last_a == 1) {
        rotary_updated += update_btn(1, 1, rotary_gpio_a.report_btn, rotary_gpio_a.btn_idx, rotary_gpio_a.bit_idx);
        rotary_updated += update_btn(0, 1, rotary_gpio_b.report_btn, rotary_gpio_b.btn_idx, rotary_gpio_b.bit_idx);
    } else if (a == 1 && b == 1) {
        rotary_updated += update_btn(0, 1, rotary_gpio_a.report_btn, rotary_gpio_a.btn_idx, rotary_gpio_a.bit_idx);
        rotary_updated += update_btn(0, 1, rotary_gpio_b.report_btn, rotary_gpio_b.btn_idx, rotary_gpio_b.bit_idx);
    }

    last_a = a;
    last_b = b;
}

int update_btn(uint8_t state, uint8_t pressed_value, uint8_t *btns, int btn_idx, int bit_idx) {
    // sets the bit to 0, effectively resetting it
    *btns &= ~(1 << bit_idx);

    uint8_t updated = 0;
    if (state == pressed_value) {
        *btns |= 1 << bit_idx;
        if (buttons[btn_idx] != 1) {
            buttons[btn_idx] = 1;
            updated = 1;
        }
    } else if (buttons[btn_idx] == 1) {
        buttons[btn_idx] = 0;
        updated = 1;
    }

    return updated;
}

int update_gpio(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx) {
    uint8_t gpio_state = gpio_get(gpio_num);
    return update_btn(gpio_state, 0, btns, btn_idx, bit_idx);
}

int update_gpio_with_pressed(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx, uint8_t pressed_value) {
    uint8_t gpio_state = gpio_get(gpio_num);
    return update_btn(gpio_state, pressed_value, btns, btn_idx, bit_idx);
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // This example doesn't use multiple report and report ID
    (void) report_id;
    (void) report_type;
    //(void) buffer;
    //(void) bufsize;

    // echo back anything we received from host
    tud_hid_report(0, buffer, bufsize);
}
