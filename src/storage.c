#include <stdio.h>
#include "pico/stdlib.h"
#include "pet.h"

void task_storage(void *arg)
{
    for (;;)
    {
        /* TODO: usar pico_flash para salvar gPet a cada 60 s */
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
