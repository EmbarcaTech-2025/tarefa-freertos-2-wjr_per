#include "pico/stdlib.h"
#include "pet.h"

/* protótipos das tasks */
void task_input(void*), task_pet_logic(void*), task_light(void*);
void task_matrix(void*), task_oled(void*), task_rgb(void*), task_storage(void*);

/* ------------------------------------------------------------------ */
pet_state_t       gPet  = { .age_sec = 0, .hunger = 0, .fun = 10, .sleeping = false };
EventGroupHandle_t egPet;
SemaphoreHandle_t  mtxPet;
/* ------------------------------------------------------------------ */

int main(void)
{
    stdio_init_all();

    egPet = xEventGroupCreate();
    mtxPet = xSemaphoreCreateMutex();

    /* ----------- Tarefas ----------- */
    xTaskCreate(task_input,      "INPUT",   256, NULL, 3, NULL);
    xTaskCreate(task_pet_logic,  "LOGIC",   512, NULL, 2, NULL);
    xTaskCreate(task_light,      "LIGHT",   256, NULL, 2, NULL);
    xTaskCreate(task_matrix,     "MATRIX",  256, NULL, 1, NULL);
    xTaskCreate(task_oled,       "OLED",    512, NULL, 1, NULL);
    xTaskCreate(task_rgb,        "RGB",     256, NULL, 0, NULL);
    xTaskCreate(task_storage,    "FLASH",   512, NULL, 0, NULL);

    vTaskStartScheduler();
    while (true) { /* nunca chega aqui */ }
}