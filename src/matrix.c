/*---------------------------------------------------------------
 *  Driver da matriz WS2812 (5 × 5)  +  minigame SNAKE
 *--------------------------------------------------------------*/
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"

#include "pet.h"
#include "frames.h"
#include "rgb.h"
#include "snake.h"

#define WS_PIN        7
#define IS_RGBW       false
#define SNAKE_STEP_US 150000        /* 150 ms entre passos      */

/* ───────── PIO ───────────────────────────────────────────────*/
static PIO  pio = pio0;
static uint sm;

static inline void put_pixel(uint32_t grb) { pio_sm_put_blocking(pio, sm, grb << 8u); }

/* ───────── Task MATRIX ──────────────────────────────────────*/
void task_matrix(void *arg)
{
    /* --- inicializa WS2812 ---------------------------------- */
    uint off = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, off, WS_PIN, 800000, IS_RGBW);

    /* temporizações */
    uint64_t last_move_us  = 0;   /* cobra → 150 ms          */
    uint64_t last_fun_us   = 0;   /* FUN   ↑ 1  s            */
    uint8_t  fun_snapshot  = 0;   /* piso provisório         */

    for (;;)
    {
        EventBits_t ev = xEventGroupWaitBits(
            egPet, EVT_TICK | EVT_SNAKE_START,
            pdTRUE, pdFALSE, portMAX_DELAY);

        /* ─── inicia novo jogo ───────────────────────────────*/
        if (ev & EVT_SNAKE_START) {
            snake_start();
            uint64_t now = time_us_64();
            last_move_us = now;
            last_fun_us  = now;

            xSemaphoreTake(mtxPet, portMAX_DELAY);
            gPet.fun = 0;               /* FUN parte de zero      */
            fun_snapshot = 0;
            xSemaphoreGive(mtxPet);
        }

        uint64_t now_us = time_us_64();

        /* ─── avança a cobrinha ──────────────────────────────*/
        if (snake_is_running() && now_us - last_move_us >= SNAKE_STEP_US) {
            snake_tick();
            last_move_us = now_us;
        }

        /* ─── FUN ↑ 1/s – nunca deixa cair durante o jogo ───*/
        if (snake_is_running()) {

            /* acorda o pet somente 1x (primeiro loop do jogo) */
            static bool woke_once = false;
            if (!woke_once) { xEventGroupSetBits(egPet, EVT_WAKE); woke_once = true; }

            if (now_us - last_fun_us >= 1000000ULL) {           /* 1 s */
                xSemaphoreTake(mtxPet, portMAX_DELAY);
                if (gPet.fun < 10) gPet.fun++;
                fun_snapshot = gPet.fun;                          /* novo piso */
                xSemaphoreGive(mtxPet);
                last_fun_us = now_us;
            } else {
                /* devolve FUN para o piso, se outra task baixou */
                xSemaphoreTake(mtxPet, portMAX_DELAY);
                if (gPet.fun < fun_snapshot)
                    gPet.fun = fun_snapshot;
                xSemaphoreGive(mtxPet);
            }
        }

        /* ─── Renderização ───────────────────────────────────*/
        if (snake_is_running()) {
            snake_draw(put_pixel);
        } else {

            /* rosto (feliz / triste / dormindo) ───────────── */
            xSemaphoreTake(mtxPet, portMAX_DELAY);
            bool good   = (gPet.hunger <= 7) && (gPet.fun >= 3);
            bool asleep =  gPet.sleeping;
            xSemaphoreGive(mtxPet);

            const uint32_t *frame = good ? frame_happy : frame_sad;

            for (int row = 0; row < 5; row++)
                for (int col = 0; col < 5; col++) {
                    int idx_l = (4 - row) * 5 + col;
                    uint32_t pix = asleep ? RGB_U32(0,0,6) : frame[idx_l];
                    put_pixel(pix);
                }
        }

        sleep_us(60);                           /* latch ≥50 µs */
    }
}
