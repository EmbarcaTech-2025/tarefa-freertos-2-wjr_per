#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <stdint.h>

void  snake_start(void);        /* inicia o mini-game             */
bool  snake_tick(void);         /* avança 1 passo; --> false = game-over */
bool  snake_is_running(void);   /* consulta estado                */

/* o matrix.c entrega um ponteiro para enviar cada pixel */
void  snake_draw(void (*put_pixel)(uint32_t));

#endif
