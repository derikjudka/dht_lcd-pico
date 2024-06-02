cmake_minimum_required(VERSION 3.13)

set(PROJ_NAME "dht_lcd-pico")

# Include the pico_sdk_import.cmake file
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(${PROJ_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

# Define the project executable and source files
add_executable(${PROJ_NAME}
    src/main.c
    src/lib/dht/dht.c
    src/lib/lcd/lcd_16x2.c
)

# Include the directories for header files
target_include_directories(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/lib/dht
    ${CMAKE_CURRENT_LIST_DIR}/src/lib/lcd
    ${PICO_SDK_PATH}
)

# Link the necessary libraries from the Pico SDK
target_link_libraries(${PROJ_NAME} pico_stdlib)

# Create map/bin/hex file etc.
pico_add_extra_outputs(${PROJ_NAME})
