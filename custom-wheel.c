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

#include "button_input.h"

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
            data_received = multicore_fifo_pop_blocking();
            report_local.buttons_a = (uint8_t)(data_received & 0xFF);
            report_local.buttons_b = (uint8_t)((data_received >> 8) & 0xFF);
            report_local.buttons_c = (uint8_t)((data_received >> 16) & 0xFF);
            if (tud_hid_ready()) {
                tud_hid_report(0, &report, sizeof(report));
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
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    //ioExpander_init();

    //for (int i = 0; i < 10000; i++) {
    //    uint8_t ok = 10;
    //    sleep_ms(ok);
    //}

    gpio_init(BTN_01);
    gpio_set_dir(BTN_01, GPIO_IN);
    gpio_pull_up(BTN_01);

    gpio_init(BTN_02);
    gpio_set_dir(BTN_02, GPIO_IN);
    gpio_pull_up(BTN_02);

    gpio_init(BTN_03);
    gpio_set_dir(BTN_03, GPIO_IN);
    gpio_pull_up(BTN_03);

    gpio_init(BTN_04);
    gpio_set_dir(BTN_04, GPIO_IN);
    gpio_pull_up(BTN_04);

    gpio_init(BTN_05);
    gpio_set_dir(BTN_05, GPIO_IN);
    gpio_pull_up(BTN_05);

    gpio_init(BTN_06);
    gpio_set_dir(BTN_06, GPIO_IN);
    gpio_pull_up(BTN_06);

    gpio_init(BTN_07);
    gpio_set_dir(BTN_07, GPIO_IN);
    gpio_pull_up(BTN_07);

    gpio_init(BTN_08);
    gpio_set_dir(BTN_08, GPIO_IN);
    gpio_pull_up(BTN_08);

    gpio_init(TOGGLE_01_A);
    gpio_set_dir(TOGGLE_01_A, GPIO_IN);
    gpio_pull_up(TOGGLE_01_A);

    gpio_init(TOGGLE_01_B);
    gpio_set_dir(TOGGLE_01_B, GPIO_IN);
    gpio_pull_up(TOGGLE_01_B);

    gpio_init(TOGGLE_02_A);
    gpio_set_dir(TOGGLE_02_A, GPIO_IN);
    gpio_pull_up(TOGGLE_02_A);

    gpio_init(TOGGLE_02_B);
    gpio_set_dir(TOGGLE_02_B, GPIO_IN);
    gpio_pull_up(TOGGLE_02_B);

    gpio_init(SHIFT_UP);
    gpio_set_dir(SHIFT_UP, GPIO_IN);
    gpio_pull_up(SHIFT_UP);

    gpio_init(SHIFT_DOWN);
    gpio_set_dir(SHIFT_DOWN, GPIO_IN);
    gpio_pull_up(SHIFT_DOWN);

    gpio_init(ROTARY_01_A);
    gpio_set_dir(ROTARY_01_A, GPIO_IN);
    gpio_pull_up(ROTARY_01_A);

    gpio_init(ROTARY_01_B);
    gpio_set_dir(ROTARY_01_B, GPIO_IN);
    gpio_pull_up(ROTARY_01_B);

    gpio_init(ROTARY_01_C);
    gpio_set_dir(ROTARY_01_C, GPIO_IN);
    gpio_pull_up(ROTARY_01_C);

    gpio_init(ROTARY_02_A);
    gpio_set_dir(ROTARY_02_A, GPIO_IN);
    gpio_pull_up(ROTARY_02_A);

    gpio_init(ROTARY_02_B);
    gpio_set_dir(ROTARY_02_B, GPIO_IN);
    gpio_pull_up(ROTARY_02_B);

    gpio_init(ROTARY_02_C);
    gpio_set_dir(ROTARY_02_C, GPIO_IN);
    gpio_pull_up(ROTARY_02_C);

    gpio_init(ROTARY_03_A);
    gpio_set_dir(ROTARY_03_A, GPIO_IN);
    gpio_pull_up(ROTARY_03_A);

    gpio_init(ROTARY_03_B);
    gpio_set_dir(ROTARY_03_B, GPIO_IN);
    gpio_pull_up(ROTARY_03_B);

    gpio_init(ROTARY_03_C);
    gpio_set_dir(ROTARY_03_C, GPIO_IN);
    gpio_pull_up(ROTARY_03_C);

    rotary_gpios[ROTARY_01_A].gpio_a = ROTARY_01_A;
    rotary_gpios[ROTARY_01_A].gpio_b = ROTARY_01_B;
    rotary_gpios[ROTARY_01_A].bit_idx = 0;
    rotary_gpios[ROTARY_01_A].btn_idx = 8;
    rotary_gpios[ROTARY_01_A].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_01_B].gpio_a = ROTARY_01_A;
    rotary_gpios[ROTARY_01_B].gpio_b = ROTARY_01_B;
    rotary_gpios[ROTARY_01_B].bit_idx = 1;
    rotary_gpios[ROTARY_01_B].btn_idx = 9;
    rotary_gpios[ROTARY_01_B].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_01_C].gpio_a = ROTARY_01_C;
    rotary_gpios[ROTARY_01_C].bit_idx = 2;
    rotary_gpios[ROTARY_01_C].btn_idx = 10;
    rotary_gpios[ROTARY_01_C].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_02_A].gpio_a = ROTARY_02_A;
    rotary_gpios[ROTARY_02_A].gpio_b = ROTARY_02_B;
    rotary_gpios[ROTARY_02_A].bit_idx = 3;
    rotary_gpios[ROTARY_02_A].btn_idx = 11;
    rotary_gpios[ROTARY_02_A].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_02_B].gpio_a = ROTARY_02_A;
    rotary_gpios[ROTARY_02_B].gpio_b = ROTARY_02_B;
    rotary_gpios[ROTARY_02_B].bit_idx = 4;
    rotary_gpios[ROTARY_02_B].btn_idx = 12;
    rotary_gpios[ROTARY_02_B].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_02_C].gpio_a = ROTARY_02_C;
    rotary_gpios[ROTARY_02_C].bit_idx = 5;
    rotary_gpios[ROTARY_02_C].btn_idx = 13;
    rotary_gpios[ROTARY_02_C].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_03_A].gpio_a = ROTARY_03_A;
    rotary_gpios[ROTARY_03_A].gpio_b = ROTARY_03_B;
    rotary_gpios[ROTARY_03_A].bit_idx = 6;
    rotary_gpios[ROTARY_03_A].btn_idx = 14;
    rotary_gpios[ROTARY_03_A].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_03_B].gpio_a = ROTARY_03_A;
    rotary_gpios[ROTARY_03_B].gpio_b = ROTARY_03_B;
    rotary_gpios[ROTARY_03_B].bit_idx = 7;
    rotary_gpios[ROTARY_03_B].btn_idx = 15;
    rotary_gpios[ROTARY_03_B].report_btn = &report.buttons_b;

    rotary_gpios[ROTARY_03_C].gpio_a = ROTARY_03_C;
    rotary_gpios[ROTARY_03_C].bit_idx = 0;
    rotary_gpios[ROTARY_03_C].btn_idx = 16;
    rotary_gpios[ROTARY_03_C].report_btn = &report.buttons_c;

    gpio_set_irq_enabled_with_callback(ROTARY_01_A, 0b1100, true, handle_rotary_state_change);
    gpio_set_irq_enabled_with_callback(ROTARY_01_B, 0b1100, true, handle_rotary_state_change);

    gpio_set_irq_enabled_with_callback(ROTARY_02_A, 0b1100, true, handle_rotary_state_change);
    gpio_set_irq_enabled_with_callback(ROTARY_02_B, 0b1100, true, handle_rotary_state_change);

    gpio_set_irq_enabled_with_callback(ROTARY_03_A, 0b1100, true, handle_rotary_state_change);
    gpio_set_irq_enabled_with_callback(ROTARY_03_B, 0b1100, true, handle_rotary_state_change);

    board_init();

    while (1) {
        uint8_t updated = 0;
        updated += update_gpio(BTN_01, &report.buttons_a, 0, 0);
        updated += update_gpio(BTN_02, &report.buttons_a, 1, 1);
        updated += update_gpio(BTN_03, &report.buttons_a, 2, 2);
        updated += update_gpio(BTN_04, &report.buttons_a, 3, 3);
        updated += update_gpio(BTN_05, &report.buttons_a, 4, 4);
        updated += update_gpio(BTN_06, &report.buttons_a, 5, 5);
        updated += update_gpio(BTN_07, &report.buttons_a, 6, 6);
        updated += update_gpio(BTN_08, &report.buttons_a, 7, 7);

        updated += update_gpio(TOGGLE_01_A, &report.buttons_c, 17, 1);
        updated += update_gpio(TOGGLE_01_B, &report.buttons_c, 18, 2);
        updated += update_gpio(TOGGLE_02_A, &report.buttons_c, 19, 3);
        updated += update_gpio(TOGGLE_02_B, &report.buttons_c, 20, 4);
        updated += update_gpio(SHIFT_UP, &report.buttons_c, 21, 5);
        updated += update_gpio(SHIFT_DOWN, &report.buttons_c, 22, 6);

        rotary_gpio_t rotary_gpio = rotary_gpios[ROTARY_01_C];
        updated += update_gpio_with_pressed(rotary_gpio.gpio_a, rotary_gpio.report_btn, rotary_gpio.btn_idx, rotary_gpio.bit_idx, 0);

        rotary_gpio = rotary_gpios[ROTARY_02_C];
        updated += update_gpio_with_pressed(rotary_gpio.gpio_a, rotary_gpio.report_btn, rotary_gpio.btn_idx, rotary_gpio.bit_idx, 0);

        rotary_gpio = rotary_gpios[ROTARY_03_C];
        updated += update_gpio_with_pressed(rotary_gpio.gpio_a, rotary_gpio.report_btn, rotary_gpio.btn_idx, rotary_gpio.bit_idx, 0);

        if (updated > 0 || rotary_updated > 0) {
            rotary_updated = 0;
            uint32_t report_send;
            report_send = (report.buttons_a) | (report.buttons_b << 8) | (report.buttons_c << 16);
            multicore_fifo_push_blocking(report_send);
            //__breakpoint();
        }
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
