#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#include "pet.h"
#include "frames.h"     /* frame_happy / frame_sad */
#include "rgb.h"        /* rgb_u32 helper          */

#define WS_PIN   7
#define IS_RGBW  false

static PIO   pio = pio0;
static uint  sm;

/* Envia um pixel no formato GRB (8 bits de deslocamento exigido pelo PIO) */
static inline void put_pixel(uint32_t grb)
{
    pio_sm_put_blocking(pio, sm, grb << 8u);
}

/* ------------------------------------------------------------------ */
/*  Tarefa Matrix: mostra rosto feliz/triste e azul quando dormindo    */
/* ------------------------------------------------------------------ */
void task_matrix(void *arg)
{
    /* --- inicializa WS2812 ---------------------------------------- */
    uint off = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, off, WS_PIN, 800000, IS_RGBW);

    for (;;)
    {
        /* espera EVT_TICK (1 Hz) */
        xEventGroupWaitBits(egPet, EVT_TICK, pdTRUE, pdFALSE, portMAX_DELAY);

        /* lê estado do pet */
        xSemaphoreTake(mtxPet, portMAX_DELAY);
        bool good   = (gPet.hunger <= 7) && (gPet.fun >= 3);
        bool asleep = gPet.sleeping;
        xSemaphoreGive(mtxPet);

        const uint32_t *frame = good ? frame_happy : frame_sad;

        /* percorre a matriz em zigue-zague */
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {

                /* índice lógico (row-major) */
                int idx_l = (4 - row) * 5 + col;

                /* zigue-zague: linhas ímpares vão da direita p/ esquerda */
                int col_phys = (row & 1) ? col : (4 - col);
                int idx_p    = row * 5 + col_phys;   /* posição física   */

                uint32_t pix = asleep
                               ? rgb_u32(0, 0, 6)     /* azul sólido      */
                               : frame[idx_l];        /* rosto            */

                put_pixel(pix);
            }
        }
        sleep_us(60);   /* latch dos WS2812 (≥50 µs) */
    }
}
