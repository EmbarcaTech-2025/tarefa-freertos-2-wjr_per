#ifndef RGB_H
#define RGB_H
#include <stdint.h>

/* MACRO sempre-constante (GRB<<8) ― pode ser usada em inicializadores */
#define RGB_U32(r,g,b) ( ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (b) )

/* Versão função (azulejada) para código dinâmico */
static inline uint32_t rgb_u32(uint8_t r,uint8_t g,uint8_t b)
{
    return RGB_U32(r,g,b);
}
#endif /* RGB_H */
