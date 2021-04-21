//
// Created by p209287 on 26/03/2021.
//

#include "pico/stdlib.h"
#include "Matrix.h"

#define COL_0 0
#define COL_1 1
#define COL_2 2
#define COL_3 3

#define ROW_0 4
#define ROW_1 5
#define ROW_2 6
#define ROW_3 7

static const uint8_t columns[4] = {COL_0, COL_1, COL_2, COL_3};
static const uint8_t rows[4] = {ROW_0, ROW_1, ROW_2, ROW_3};

static void ConfigureColumns(void)
{
    uint8_t i = 0;
    for(i = 0; i < 4; i++)
    {
        gpio_init(columns[i]);
        gpio_set_dir(columns[i], GPIO_IN);
    }
}

static void ConfigureRows(void)
{
    uint8_t i = 0;
    for(i = 0; i < 4; i++)
    {
        gpio_init(rows[i]);
        gpio_set_dir(rows[i], GPIO_OUT);
    }
}

static uint8_t ReadColumn(void)
{
    uint8_t data = 0;
    for(int i = 0; i < 4; i++)
    {
        data |= (gpio_get(columns[i]) << i);
    }
    return data;
}

void Matrix_Initialise(void)
{
    ConfigureColumns();
    ConfigureRows();
}

uint8_t Matrix_Read(uint32_t *destination)
{
    gpio_put(rows[0], true);
    *destination = (uint32_t)ReadColumn();
    gpio_put(rows[0], false);

//    gpio_put(rows[1], true);
//    *destination = ReadColumn() << 4;
//    gpio_put(rows[1], false);
//
//    gpio_put(rows[2], true);
//    destination[1] = ReadColumn();
//    gpio_put(rows[2], false);
//
//    gpio_put(rows[3], true);
//    destination[1] = ReadColumn() << 4;
//    gpio_put(rows[3], false);
    return 1;
}