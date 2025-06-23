#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "pet.h"

#define WS_PIN 7
#define IS_RGBW false

static PIO pio = pio0;
static uint sm;

static inline void put_pixel(uint32_t grb){ pio_sm_put_blocking(pio, sm, grb<<8u); }
static inline uint32_t rgb_u32(uint8_t r,uint8_t g,uint8_t b){ return ((r<<8)|(g<<16)|b); }

void task_matrix(void *arg)
{
    uint off = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, off, WS_PIN, 800000, IS_RGBW);

    for (;;)
    {
        xEventGroupWaitBits(egPet, EVT_TICK, pdTRUE, pdFALSE, portMAX_DELAY);

        uint32_t color;
        xSemaphoreTake(mtxPet, portMAX_DELAY);
        if (gPet.sleeping)          color = rgb_u32(0,0,2);
        else if (gPet.fun < 3)      color = rgb_u32(5,0,0);
        else if (gPet.hunger > 7)   color = rgb_u32(5,2,0);
        else                        color = rgb_u32(0,5,0);
        xSemaphoreGive(mtxPet);

        for (int i = 0; i < 25; i++) put_pixel(color);
    }
}
