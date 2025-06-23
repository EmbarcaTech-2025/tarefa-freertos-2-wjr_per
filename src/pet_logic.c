#include <stdio.h>
#include "pico/stdlib.h"
#include "pet.h"

void task_pet_logic(void *arg)
{
    /* LED de debug: acende vermelho quando task inicia */
    gpio_init(13); gpio_set_dir(13, GPIO_OUT); gpio_put(13, 1);

    for (;;)
    {
        /* 1) Espera eventos FEED/PLAY/SLEEP/WAKE (limpa ao sair) */
        EventBits_t ev = xEventGroupWaitBits(
            egPet,
            EVT_FEED | EVT_PLAY | EVT_SLEEP | EVT_WAKE,
            pdTRUE,          /* clear bits */
            pdFALSE,
            pdMS_TO_TICKS(1000)   /* timeout ~1 s p/ gerar TICK */
        );

        xSemaphoreTake(mtxPet, portMAX_DELAY);

        if (ev & EVT_FEED)  gPet.hunger = 0;
        if (ev & EVT_PLAY)  gPet.fun    = 10;
        if (ev & EVT_SLEEP) gPet.sleeping = true;
        if (ev & EVT_WAKE)  gPet.sleeping = false;

        /* 2) Tick de 1 s: envelhece / degrada atributos */
        gPet.age_sec++;
        if (!gPet.sleeping) {
            if (gPet.hunger < 10) gPet.hunger++;
            if (gPet.fun    > 0)  gPet.fun--;
        }
        xSemaphoreGive(mtxPet);

        /* 3) Broadcast de atualização => OLED, matriz, RGB */
        xEventGroupSetBits(egPet, EVT_TICK);
    }
}
