# Pull in Raspberry Pi Pico SDK (must be before project)
if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")
endif ()

if (NOT EXISTS ${PICO_SDK_PATH}/CMakeLists.txt)
    message(STATUS "Cloning Pico SDK into ${PICO_SDK_PATH}")
    execute_process(COMMAND git clone https://github.com/raspberrypi/pico-sdk ${PICO_SDK_PATH})
    execute_process(COMMAND git submodule update --init
                    WORKING_DIRECTORY ${PICO_SDK_PATH})
endif ()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
