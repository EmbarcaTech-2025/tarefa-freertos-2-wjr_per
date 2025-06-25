/*  task_pet_logic.c
 *  Lógica central do bichinho + geração do EVT_TICK (1 Hz)
 *  --------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#include "pet.h"

/* ——— protótipo do callback ———————————————————————— */
static bool tick_cb(struct repeating_timer *t);

/* ——— handle do timer —————————————————————————————— */
static struct repeating_timer tim1s;

/* ——— callback de 1 Hz ———————————————————————————— */
/* Executa em IRQ context – só pequenas ações!           */
static bool tick_cb(struct repeating_timer *t)
{
    BaseType_t task_woken = pdFALSE;
    /* publica EVT_TICK para as tasks de user-land          */
    xEventGroupSetBitsFromISR(egPet, EVT_TICK, &task_woken);
    return true;                 /* repete indefinidamente  */
}

/* ——— TASK ------------------------------------------------ */
void task_pet_logic(void *arg)
{
    /* LED de debug: acende vermelho quando a task inicia    */
    gpio_init(13); gpio_set_dir(13, GPIO_OUT); gpio_put(13, 1);

    /* instala timer de 1 s (negativo = periódico)           */
    add_repeating_timer_us(-1000000, tick_cb, NULL, &tim1s);

    for (;;) {

        /* espera qualquer evento de interesse -------------- */
        EventBits_t ev = xEventGroupWaitBits(
            egPet,
            EVT_FEED | EVT_PLAY | EVT_SLEEP | EVT_WAKE | EVT_TICK,
            pdTRUE,          /* ← limpa os bits recebidos      */
            pdFALSE,
            portMAX_DELAY);  /* bloqueia até algo acontecer    */

        xSemaphoreTake(mtxPet, portMAX_DELAY);

        /* ——— ações imediatas vindas dos botões ——— */
        if (ev & EVT_FEED)  gPet.hunger   = 0;
        if (ev & EVT_PLAY)  gPet.fun      = 10;
        if (ev & EVT_SLEEP) gPet.sleeping = true;
        if (ev & EVT_WAKE)  gPet.sleeping = false;

        /* ——— a cada TICK (1 s) ———————————————— */
        if (ev & EVT_TICK) {
            gPet.age_sec++;

            if (!gPet.sleeping) {
                if (gPet.hunger < 10) gPet.hunger++;
                if (gPet.fun    >  0) gPet.fun--;
            }
        }

        xSemaphoreGive(mtxPet);
        
    }
}

