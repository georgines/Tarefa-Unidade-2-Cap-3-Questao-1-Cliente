# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(tarefau2c31_cliente C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c" "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable(tarefau2c31_cliente ${SOURCES})

pico_set_program_name(tarefau2c31_cliente "tarefau2c31_cliente")
pico_set_program_version(tarefau2c31_cliente "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(tarefau2c31_cliente 1)
pico_enable_stdio_usb(tarefau2c31_cliente 0)

# Add the standard library to the build
target_link_libraries(tarefau2c31_cliente
pico_stdlib
pico_cyw43_arch_lwip_threadsafe_background
hardware_gpio
hardware_adc
)

# Add the standard include files to the build
target_include_directories(tarefau2c31_cliente PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(tarefau2c31_cliente 
        
)

pico_add_extra_outputs(tarefau2c31_cliente)