#include <stdio.h>
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "pet.h"

static ssd1306_t disp;

void task_oled(void *arg)
{
    /* init I²C */
    i2c_init(i2c1, 400000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); gpio_pull_up(15);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);

    char buf[32];

    for (;;)
    {
        /* espera EVT_TICK */
        xEventGroupWaitBits(egPet, EVT_TICK, pdTRUE, pdFALSE, portMAX_DELAY);

        xSemaphoreTake(mtxPet, portMAX_DELAY);

        ssd1306_clear(&disp);
        snprintf(buf,sizeof(buf),"AGE  %lus", (unsigned long)gPet.age_sec);
        ssd1306_draw_string(&disp,0,0,1,buf);
        snprintf(buf,sizeof(buf),"HUN  %u", gPet.hunger);
        ssd1306_draw_string(&disp,0,12,1,buf);
        snprintf(buf,sizeof(buf),"FUN  %u", gPet.fun);
        ssd1306_draw_string(&disp,0,24,1,buf);
        snprintf(buf,sizeof(buf),"SLP  %s", gPet.sleeping?"YES":"NO");
        ssd1306_draw_string(&disp,0,36,1,buf);

        xSemaphoreGive(mtxPet);

        ssd1306_show(&disp);
    }
}
