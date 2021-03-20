# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Users/harry/Documents/Development/Home/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(custom-wheel C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

# Add executable. Default name is the project name, version 0.1

add_executable(custom-wheel custom-wheel.c usb_descriptors.c io-expander.c)

pico_set_program_name(custom-wheel "custom-wheel")
pico_set_program_version(custom-wheel "0.1")

pico_enable_stdio_uart(custom-wheel 0)
pico_enable_stdio_usb(custom-wheel 0)

# Add the standard library to the build
target_link_libraries(custom-wheel pico_stdlib)

# Add any user requested libraries
target_link_libraries(custom-wheel
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_timer
        pico_stdlib
        tinyusb_device
        tinyusb_board
        )

pico_add_extra_outputs(custom-wheel)

target_include_directories(custom-wheel PRIVATE ${CMAKE_CURRENT_LIST_DIR})

