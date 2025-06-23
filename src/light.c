#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pet.h"

#define LDR_GPIO 28
#define LDR_CH   2

void task_light(void *arg)
{
    adc_init();
    adc_gpio_init(LDR_GPIO);
    adc_select_input(LDR_CH);

    bool dark_prev = false;

    for (;;)
    {
        uint16_t raw = adc_read();   // 0-4095
        bool dark = raw < 1000;      // limiar simples

        if (dark != dark_prev) {
            xEventGroupSetBits(egPet, dark ? EVT_SLEEP : EVT_WAKE);
            dark_prev = dark;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
