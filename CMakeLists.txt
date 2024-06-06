# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
   set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(PICO_SDK_PATH ${USERHOME}/.pico-sdk/sdk/1.5.1)
set(PICO_TOOLCHAIN_PATH ${USERHOME}/.pico-sdk/toolchain/13_2_Rel1)
if(WIN32)
    set(pico-sdk-tools_DIR ${USERHOME}/.pico-sdk/tools/1.5.1)
    include(${pico-sdk-tools_DIR}/pico-sdk-tools-config.cmake)
    include(${pico-sdk-tools_DIR}/pico-sdk-tools-config-version.cmake)
endif()
# ====================================================================================

set(PICO_BOARD waveshare_rp2040_plus_4mb CACHE STRING "Board type")
set(FAMILY rp2040)
set(BOARD pico_sdk)
set(CFG_TUSB_BOARD rp2040)


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(analog_buddy C CXX ASM)

add_subdirectory(vendor/pico-ssd1306)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/usb_descriptors.cpp
  )

pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
  CFG_TUSB_CONFIG_FILE="tusb_config.hpp"
)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
  hardware_i2c
  pico_multicore
  pico_ssd1306
  pico_stdlib
  pico_unique_id
  tinyusb_device
  tinyusb_board
)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/vendor
  # ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(${PROJECT_NAME})

