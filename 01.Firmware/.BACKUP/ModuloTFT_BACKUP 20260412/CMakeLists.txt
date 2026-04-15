# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == PICO SDK ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)

set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(ModuloTFT C CXX ASM)

pico_sdk_init()


# =========================
# LVGL
# =========================

add_subdirectory(lvgl)

# 👉 IMPORTANTE: lv_conf para LVGL
target_include_directories(lvgl PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(lvgl PUBLIC
    LV_CONF_INCLUDE_SIMPLE
)

# =========================
# FUENTES
# =========================

file(GLOB_RECURSE EEZ_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/eez/*.c
    ${CMAKE_CURRENT_LIST_DIR}/eez/*.cpp
)

list(FILTER EEZ_SOURCES EXCLUDE REGEX "eez/core/.*")
list(FILTER EEZ_SOURCES EXCLUDE REGEX "eez/gui/.*")
list(FILTER EEZ_SOURCES EXCLUDE REGEX "eez/libs/.*")
list(FILTER EEZ_SOURCES EXCLUDE REGEX "eez/platforms/.*")
list(FILTER EEZ_SOURCES EXCLUDE REGEX "eez/flow/.*")

file(GLOB UI_SOURCES
    ui/*.c
    ui/*.cpp
    ui/screens/*.c
    ui/components/*.c
    ui/images/*.c
)


#target_compile_options(lvgl PRIVATE 
#    -include ${CMAKE_CURRENT_LIST_DIR}/lv_conf.h
#)


target_link_libraries(lvgl PUBLIC pico_stdlib)

# =========================
# EJECUTABLE
# =========================

add_executable(ModuloTFT 
    ModuloTFT.c
    display_driver.c
    touch_driver.c
    ${UI_SOURCES}
    ${EEZ_SOURCES}
)


#target_compile_options(ModuloTFT PRIVATE 
#    -include ${CMAKE_CURRENT_LIST_DIR}/lv_conf.h
#)

# Definiciones
target_compile_definitions(ModuloTFT PRIVATE 
    LV_CONF_INCLUDE_SIMPLE
    EEZ_PLATFORM_PICO
    EEZ_FOR_LVGL
    EEZ_PLATFORM_NO_OS
    EEZ_NO_AGG
    EEZ_SKIP_GUI_DISPLAY
    EEZ_CONF_NOSYS
    EEZ_SKIP_CRYPTO
    CUSTOM_OPERATION_TYPES_H
)

# =========================
# INCLUDES
# =========================

target_include_directories(ModuloTFT PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/ui
    ${CMAKE_CURRENT_LIST_DIR}/eez
)

# =========================
# LIBS
# =========================

target_link_libraries(ModuloTFT
    pico_stdlib
    hardware_spi
    hardware_gpio
    lvgl
)


# STDIO
pico_enable_stdio_uart(ModuloTFT 0)
pico_enable_stdio_usb(ModuloTFT 1)

pico_set_program_name(ModuloTFT "ModuloTFT")
pico_set_program_version(ModuloTFT "0.1")

pico_add_extra_outputs(ModuloTFT)