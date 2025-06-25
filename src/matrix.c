#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#include "pet.h"
#include "frames.h"
#include "rgb.h"
#include "snake.h"

#define WS_PIN   7
#define IS_RGBW  false

static PIO   pio = pio0;
static uint  sm;

static inline void put_pixel(uint32_t grb)
{
    pio_sm_put_blocking(pio, sm, grb << 8u);
}

/* ────────────────────────────────────────────
 *  Task MATRIX: rosto, mini-game ou “sono”
 * ──────────────────────────────────────────── */
void task_matrix(void *arg)
{
    uint off = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, off, WS_PIN, 800000, IS_RGBW);

    static uint64_t last_us = 0;
    static bool     prev_snake = false;

    for (;;)
    {
        /* acorda no EVT_TICK ou após 20 ms – garante FPS alto ao jogo */
        xEventGroupWaitBits(egPet, EVT_TICK, pdTRUE, pdFALSE,
                            pdMS_TO_TICKS(20));

        /* tempo de passo da cobra (120 ms) */
        if (snake_is_running()) {
            uint64_t now = time_us_64();
            if (now - last_us >= 700000) {
                snake_tick();
                last_us = now;
            }
        }

        /* estado do pet ------------------------------------------- */
        xSemaphoreTake(mtxPet, portMAX_DELAY);
        bool good   = (gPet.hunger <= 7) && (gPet.fun >= 3);
        bool asleep = gPet.sleeping;

        if (prev_snake && !snake_is_running()) {      /* game-over → +FUN */
            if (gPet.fun < 10) ++gPet.fun;
        }
        prev_snake = snake_is_running();
        xSemaphoreGive(mtxPet);

        /* desenho -------------------------------------------------- */
        if (snake_is_running()) {
            snake_draw(put_pixel);
        } else {
            const uint32_t *frame = good ? frame_happy : frame_sad;

            for (int row = 0; row < 5; ++row) {
                for (int col = 0; col < 5; ++col) {
                    int idx_l   = (4 - row) * 5 + col;
                    int col_phys = (row & 1) ? col : (4 - col);

                    uint32_t pix = asleep
                                   ? rgb_u32(0, 0, 6)   /* azul dormindo */
                                   : frame[idx_l];

                    (void)col_phys;   /* índice físico já respeitado pelo put order */
                    put_pixel(pix);
                }
            }
        }

        sleep_us(60);               /* latch WS2812 (≥50 µs) */
    }
}
