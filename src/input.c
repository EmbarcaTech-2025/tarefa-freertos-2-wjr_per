/*  src/input.c – leitura dos botões A e B
 *  A : alimentar  (EVT_FEED)
 *  B : minigame da cobrinha
 *  A+B : dormir / acordar (EVT_SLEEP / EVT_WAKE)
 */
#include "pico/stdlib.h"
#include "pet.h"
#include "snake.h"

#define BTN_A_PIN  5
#define BTN_B_PIN  6

static void init_buttons(void)
{
    gpio_init(BTN_A_PIN); gpio_set_dir(BTN_A_PIN, GPIO_IN); gpio_pull_up(BTN_A_PIN);
    gpio_init(BTN_B_PIN); gpio_set_dir(BTN_B_PIN, GPIO_IN); gpio_pull_up(BTN_B_PIN);
}

void task_input(void *arg)
{
    init_buttons();

    bool prevA = true, prevB = true;
    bool sleeping = false;

    for (;;)
    {
        bool a = gpio_get(BTN_A_PIN);   /* 1 = solto, 0 = pressionado */
        bool b = gpio_get(BTN_B_PIN);

        /* A + B simultâneos → alterna sono */
        if (prevA && prevB && !a && !b) {
            if (!sleeping) { xEventGroupSetBits(egPet, EVT_SLEEP); sleeping = true; }
            else           { xEventGroupSetBits(egPet, EVT_WAKE ); sleeping = false;}
        }
        /* A sozinho → FEED */
        else if (prevA && !a &&  b) {
            xEventGroupSetBits(egPet, EVT_FEED);
        }
        /* B sozinho → inicia minigame */
        else if (prevB && !b &&  a) {
            if (!snake_is_running()) {
                snake_start();
                xEventGroupSetBits(egPet, EVT_SNAKE_START);
            }
        }

        prevA = a;
        prevB = b;
        vTaskDelay(pdMS_TO_TICKS(50));               /* debounce ~20 Hz */
    }
}
