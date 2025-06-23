#include "pico/stdlib.h"
#include "pet.h"

#define LED_R 11
#define LED_G 12
#define LED_B 13

static inline void led_rgb(uint8_t r,uint8_t g,uint8_t b){
    gpio_put(LED_R,r); gpio_put(LED_G,g); gpio_put(LED_B,b);
}

void task_rgb(void *arg)
{
    gpio_init(LED_R); gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G); gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B); gpio_set_dir(LED_B, GPIO_OUT);

    for (;;)
    {
        xEventGroupWaitBits(egPet, EVT_TICK, pdTRUE, pdFALSE, portMAX_DELAY);

        xSemaphoreTake(mtxPet, portMAX_DELAY);
        if (gPet.sleeping)          led_rgb(0,0,1);
        else if (gPet.fun < 3)      led_rgb(1,0,0);
        else if (gPet.hunger > 7)   led_rgb(1,1,0);
        else                        led_rgb(0,1,0);
        xSemaphoreGive(mtxPet);
    }
}
