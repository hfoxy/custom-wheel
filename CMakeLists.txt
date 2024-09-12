# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "C:/Users/harry/Documents/Development/Home/pico-sdk")
set(PICO_SDK_FETCH_FROM_GIT "true")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(fox-wheel C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

# Add executable. Default name is the project name, version 0.1

add_executable(fox-wheel fox-wheel.c usb_descriptors.c io-expander.c)

pico_set_program_name(fox-wheel "fox-wheel")
pico_set_program_version(fox-wheel "4.0")

pico_enable_stdio_uart(fox-wheel 0)
pico_enable_stdio_usb(fox-wheel 0)

# Add the standard library to the build
target_link_libraries(fox-wheel pico_stdlib)

# Add any user requested libraries
target_link_libraries(fox-wheel
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_timer
        pico_stdlib
        pico_multicore
        tinyusb_device
        tinyusb_board
        )

pico_add_extra_outputs(fox-wheel)

target_include_directories(fox-wheel PRIVATE ${CMAKE_CURRENT_LIST_DIR})

