#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"

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

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// GPIO defines
#define GPIO_01 28
#define GPIO_02 27
#define GPIO_03 26
#define GPIO_04 22
#define GPIO_05 21
#define GPIO_06 20
#define GPIO_07 19
#define GPIO_08 18

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

int update_gpio(int i, uint8_t *string, int i1, int i2);

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
    

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    gpio_init(GPIO_01);
    gpio_set_dir(GPIO_01, GPIO_IN);
    gpio_pull_up(GPIO_01);

    gpio_init(GPIO_02);
    gpio_set_dir(GPIO_02, GPIO_IN);
    gpio_pull_up(GPIO_02);

    gpio_init(GPIO_03);
    gpio_set_dir(GPIO_03, GPIO_IN);
    gpio_pull_up(GPIO_03);

    gpio_init(GPIO_04);
    gpio_set_dir(GPIO_04, GPIO_IN);
    gpio_pull_up(GPIO_04);

    gpio_init(GPIO_05);
    gpio_set_dir(GPIO_05, GPIO_IN);
    gpio_pull_up(GPIO_05);

    gpio_init(GPIO_06);
    gpio_set_dir(GPIO_06, GPIO_IN);
    gpio_pull_up(GPIO_06);

    gpio_init(GPIO_07);
    gpio_set_dir(GPIO_07, GPIO_IN);
    gpio_pull_up(GPIO_07);

    gpio_init(GPIO_08);
    gpio_set_dir(GPIO_08, GPIO_IN);
    gpio_pull_up(GPIO_08);


    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);

    puts("Hello, world!");

    board_init();
    tusb_init();

    while (1) {
        tud_task(); // tinyusb device task

        if (toggle == 1) {
            array[0] = 0x0;
            array[1] = 0x1;
            array[2] = 0x0;
            toggle = 0;
        } else {
            array[0] = 0x0;
            array[1] = 0x0;
            array[2] = 0x0;
            toggle = 1;
        }

        tud_hid_report(0, array, 64);
    }

    return 0;
}

int update_gpio(int gpioNum, uint8_t *btns, int btn_idx, int bit_idx) {
    uint8_t updated = 0;
    uint8_t gpio_state = gpio_get(gpioNum);
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

    // echo back anything we received from host
    tud_hid_report(0, buffer, bufsize);
}
