# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
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

project(midi_buddy C CXX ASM)

add_subdirectory(vendor/pico-ssd1306)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add image data

add_library(image_data STATIC
  build/assets/images/images.cpp
  build/assets/fonts/fonts.cpp
  )

target_include_directories(image_data PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(image_data
  pico_stdlib
)

# Run image_gs4 generator
file(GLOB IMAGES ${CMAKE_CURRENT_LIST_DIR}/assets/**/*.png)
file(GLOB JSON_FONTS ${CMAKE_CURRENT_LIST_DIR}/assets/**/*.json)
add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/images/images.hpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/images/images.cpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/fonts/fonts.hpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/fonts/fonts.cpp
  COMMAND
    python ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/tools/image_gs4
    ${CMAKE_CURRENT_LIST_DIR}/assets
    ${CMAKE_CURRENT_LIST_DIR}/build/assets
  DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/tools/image_gs4/__main__.py
    ${IMAGES}
    ${JSON_FONTS}
  COMMENT "Running image_gs4"
)

add_custom_target(
  image_gs4 ALL
  DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/images/images.hpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/images/images.cpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/fonts/fonts.hpp
    ${CMAKE_CURRENT_LIST_DIR}/build/assets/fonts/fonts.cpp
)

add_dependencies(image_data image_gs4)

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
  hardware_spi
  image_data
  pico_multicore
  pico_ssd1306
  pico_stdlib
  pico_unique_id
  tinyusb_device
  tinyusb_board
)

# Add the include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/build/assets
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/vendor
)

pico_add_extra_outputs(${PROJECT_NAME})