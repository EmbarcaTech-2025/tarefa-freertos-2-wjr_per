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
# -------------------------------------------------------------------
#  Cabeçalho que o VS Code Extension injeta  (NÃO REMOVA)
# -------------------------------------------------------------------
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
# -------------------------------------------------------------------
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# -------- FreeRTOS --------
if(DEFINED ENV{FREERTOS_PATH})
    set(FREERTOS_PATH $ENV{FREERTOS_PATH})
else()
    set(FREERTOS_PATH ${CMAKE_CURRENT_LIST_DIR}/RTOS)
endif()
message(STATUS "FreeRTOS kernel in: ${FREERTOS_PATH}")

# -------- SDK & Kernel ----
include(pico_sdk_import.cmake)
include(${FREERTOS_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(pixel_pet C CXX ASM)
pico_sdk_init()

# -------- Fontes ----------
file(GLOB SRC_FILES
    src/*.c               # tudo que você criar em src/
)

# drivers que ficaram dentro de include/
set(DRIVER_FILES
    
    include/ssd1306_i2c.c
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    ${DRIVER_FILES}
)

# -------- WS2812 PIO ------  (gera generated/ws2812.pio.h)
pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated
)

# -------- Includes --------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include      # headers + drivers
    ${CMAKE_CURRENT_LIST_DIR}/generated    # header PIO gerado
)

# -------- Libs ------------
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_adc
    hardware_pwm
    FreeRTOS-Kernel-Heap4
)

# -------- Artefatos -------
pico_add_extra_outputs(${PROJECT_NAME})

# (opcional) exporta compile_commands.json para clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
