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
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ================================================================================

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# ────────────────────────────────────────────────────────────────────────────────
# 1 ▸ Alvo / SDK
# ────────────────────────────────────────────────────────────────────────────────
set(PICO_BOARD pico CACHE STRING "Board type")
project(pixel_pet C CXX ASM)

# ────────────────────────────────────────────────────────────────────────────────
# 2 ▸ FreeRTOS
# ────────────────────────────────────────────────────────────────────────────────
if(DEFINED ENV{FREERTOS_PATH})
    set(FREERTOS_PATH $ENV{FREERTOS_PATH})
else()
    set(FREERTOS_PATH ${CMAKE_CURRENT_LIST_DIR}/RTOS)
endif()
message(STATUS "FreeRTOS kernel in: ${FREERTOS_PATH}")

include(pico_sdk_import.cmake)
include(${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
pico_sdk_init()

# ────────────────────────────────────────────────────────────────────────────────
# 3 ▸ Fontes
# ────────────────────────────────────────────────────────────────────────────────
file(GLOB SRC_FILES "src/*.c")          # tudo que estiver em src/
set(DRIVER_FILES   include/ssd1306_i2c.c)

# Executável COM sufixo .elf  (fundamental p/ UF2)
add_executable(pixel_pet.elf
    ${SRC_FILES}
    ${DRIVER_FILES}
    
)

# ────────────────────────────────────────────────────────────────────────────────
# 4 ▸ Header PIO WS2812  (gerado em generated/)
# ────────────────────────────────────────────────────────────────────────────────
pico_generate_pio_header(pixel_pet.elf
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated
)

# ────────────────────────────────────────────────────────────────────────────────
# 5 ▸ Includes
# ────────────────────────────────────────────────────────────────────────────────
target_include_directories(pixel_pet.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include      # drivers + headers
    ${CMAKE_CURRENT_LIST_DIR}/assets       # frames.h mora aqui
    ${CMAKE_CURRENT_LIST_DIR}/generated    # ws2812.pio.h gerado
)

# ────────────────────────────────────────────────────────────────────────────────
# 6 ▸ Bibliotecas
# ────────────────────────────────────────────────────────────────────────────────
target_link_libraries(pixel_pet.elf
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_adc
    hardware_pwm
    FreeRTOS-Kernel-Heap4
)

# ────────────────────────────────────────────────────────────────────────────────
# 7 ▸ Artefatos (ELF → UF2/HEX/BIN/MAP)
# ────────────────────────────────────────────────────────────────────────────────
pico_add_extra_outputs(pixel_pet.elf)

# ────────────────────────────────────────────────────────────────────────────────
# 8 ▸ compile_commands.json (clangd)
# ────────────────────────────────────────────────────────────────────────────────
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
