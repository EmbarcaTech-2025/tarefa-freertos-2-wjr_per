#include "pico/stdlib.h"
#include "pet.h"

#define BTN_A 5   // feed
#define BTN_B 6   // play

static void init_buttons(void)
{
    gpio_init(BTN_A); gpio_set_dir(BTN_A, GPIO_IN); gpio_pull_up(BTN_A);
    gpio_init(BTN_B); gpio_set_dir(BTN_B, GPIO_IN); gpio_pull_up(BTN_B);
}

void task_input(void *arg)
{
    init_buttons();
    bool prevA = true, prevB = true;

    for (;;)
    {
        bool a = gpio_get(BTN_A);
        bool b = gpio_get(BTN_B);

        if (prevA && !a) xEventGroupSetBits(egPet, EVT_FEED);
        if (prevB && !b) xEventGroupSetBits(egPet, EVT_PLAY);

        prevA = a; prevB = b;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
