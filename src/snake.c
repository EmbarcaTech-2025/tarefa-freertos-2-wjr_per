#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "snake.h"
#include "rgb.h"

/* ────────────────────────────────────────────
 *  Tamanho do tabuleiro 5 × 5  (índices 0-4)
 * ──────────────────────────────────────────── */
#define W 5
#define H 5
#define LEN_MAX (W * H)

/* ─── Joystick (eixos ADC) ─────────────────── */
#define JOY_Y_CH 0          /* pino 26 → cima / baixo             */
#define JOY_X_CH 1          /* pino 27 → esquerda / direita       */
#define TH_LOW   1500       /* zona morta central                 */
#define TH_HIGH  2600

typedef struct { int8_t x, y; } vec2;

/* ─── Estado global do mini-game ───────────── */
static vec2   snake[LEN_MAX];
static uint8_t len;
static vec2   dir;          /* direção actual  (-1/0/+1)          */
static vec2   apple;
static bool   running = false;

/* ─── Helpers ──────────────────────────────── */
static inline bool eq(vec2 a, vec2 b)           { return a.x == b.x && a.y == b.y; }
static inline int  idx(vec2 p)                  { return p.y * W + p.x; }

/* posição aleatória que não colide com a cobra */
static vec2 rnd_empty(void)
{
    while (true) {
        vec2 p = { rand() % W, rand() % H };
        bool clash = false;
        for (uint8_t i = 0; i < len; ++i)
            if (eq(p, snake[i])) { clash = true; break; }
        if (!clash) return p;
    }
}

/* ────────────────────────────────────────────
 *  API pública
 * ──────────────────────────────────────────── */
void snake_start(void)
{
    /* ADC preparado uma única vez */
    static bool adc_ok = false;
    if (!adc_ok) {
        adc_init();
        adc_gpio_init(26);        /* Y */
        adc_gpio_init(27);        /* X */
        adc_ok = true;
    }

    len = 3;
    snake[0] = (vec2){2, 2};
    snake[1] = (vec2){1, 2};
    snake[2] = (vec2){0, 2};

    dir   = (vec2){ 1, 0};       /* começa indo para a direita */
    apple = rnd_empty();

    running = true;
}

bool snake_is_running(void) { return running; }

/* ─── Leitura do joystick – ajusta ‘dir’ ───── */
static void joystick_dir(void)
{
    uint16_t rawx, rawy;
    adc_select_input(JOY_X_CH); rawx = adc_read();
    adc_select_input(JOY_Y_CH); rawy = adc_read();

    vec2 d = dir;

    if (rawx < TH_LOW)   d = (vec2){-1,  0};
    if (rawx > TH_HIGH)  d = (vec2){ 1,  0};
    if (rawy < TH_LOW)   d = (vec2){ 0, -1};
    if (rawy > TH_HIGH)  d = (vec2){ 0,  1};

    /* evita marcha-atrás instantânea */
    if (len >= 2 && eq(d, (vec2){ -(snake[0].x - snake[1].x),
                                  -(snake[0].y - snake[1].y) }))
        return;

    dir = d;
}

/* ─── Avança 1 passo – devolve false se morrer ─ */
bool snake_tick(void)
{
    if (!running) return false;

    joystick_dir();

    /* novo head */
    vec2 head = { snake[0].x + dir.x, snake[0].y + dir.y };

    /* colisão com parede ou corpo? */
    if (head.x < 0 || head.x >= W || head.y < 0 || head.y >= H) { running = false; return false; }
    for (uint8_t i = 0; i < len; ++i)
        if (eq(head, snake[i])) { running = false; return false; }

    /* shift corpo */
    for (int i = len; i > 0; --i) snake[i] = snake[i - 1];
    snake[0] = head;

    /* maçã?  */
    if (eq(head, apple)) {
        if (len < LEN_MAX) ++len;
        apple = rnd_empty();
    }

    if (len == LEN_MAX) running = false;   /* vitória preenche tabuleiro */

    return running;
}

/* ────────────────────────────────────────────
 *  Desenho – recebe callback put_pixel(GRB)
 * ──────────────────────────────────────────── */
void snake_draw(void (*put_pixel)(uint32_t))
{
    for (int y = 0; y < H; ++y) {
        for (int lx = 0; lx < W; ++lx) {
            int x = (y & 1) ? lx : (W - 1 - lx);   /* zigue-zague físico */
            vec2 p = { x, y };

            uint32_t col = RGB_U32(0, 0, 0);       /* fundo preto */

            if (eq(p, apple))
                col = RGB_U32(6, 0, 0);            /* maçã vermelha */
            else
                for (uint8_t i = 0; i < len; ++i)
                    if (eq(p, snake[i])) { col = RGB_U32(0, 6, 0); break; }

            put_pixel(col);
        }
    }
}
