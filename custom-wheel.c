#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "io-expander.h"

#include "bsp/board.h"
#include "tusb.h"

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
#define BTN_01 28
#define BTN_02 27
#define BTN_03 26
#define BTN_04 22
#define BTN_05 21
#define BTN_06 20
#define BTN_07 19
#define BTN_08 18

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}


typedef struct
{
    uint8_t buttons_a;
    uint8_t buttons_b;
    uint8_t buttons_c;
} gamepad_report_t;


static gamepad_report_t report_old;
static gamepad_report_t report;

uint8_t buttons[24];
uint8_t array[3];

int update_gpio(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx);
int update_ioe(int expander_io_num, uint8_t *btns, int btn_idx, int bit_idx);


int main()
{
    stdio_init_all();

    /*// SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);*/
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    ioExpander_init();

    for (int i = 0; i < 10000; i++) {
        uint8_t ok = 10;
        sleep_ms(ok);
    }


    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);

    puts("Hello, world!");

    //board_init();
    //tusb_init();

    while (1) {
        //tud_task(); // tinyusb device task

        // buttons are right to left
        // 8 7 6 5 4 3 2 1
        report.buttons_a = 0;
        // 16 15 14 13 12 11 10 9
        report.buttons_b = 0;
        // 24 23 22 21 20 19 18 17
        report.buttons_c = 0;

        uint8_t updated = 0;
        updated += update_ioe(14, &report.buttons_a, 0, 0);
        // updated += update_ioe(2, &report.buttons_a, 1, 1);
        // updated += update_ioe(3, &report.buttons_a, 2, 2);
        // updated += update_ioe(4, &report.buttons_a, 3, 3);
        // updated += update_ioe(5, &report.buttons_a, 4, 4);
        // updated += update_ioe(6, &report.buttons_a, 5, 5);
        // updated += update_ioe(7, &report.buttons_a, 6, 6);
        // updated += update_ioe(8, &report.buttons_a, 7, 7);

        if (updated > 0) {
            //if(tud_hid_ready())
            //{
            //    tud_hid_report(0, &report, sizeof(report));
            //}
            __breakpoint();
        }
    }

    return 0;
}

int update_gpio(int gpio_num, uint8_t *btns, int btn_idx, int bit_idx) {
    uint8_t updated = 0;
    uint8_t gpio_state = gpio_get(gpio_num);
    if (gpio_state == 0) {
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

int update_ioe(int expander_io_num, uint8_t *btns, int btn_idx, int bit_idx) {
    uint8_t updated = 0;
    uint8_t gpio_state = get_value(expander_io_num);
    if (gpio_state == 0) {
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
