#ifndef FRAMES_H
#define FRAMES_H
#include <stdint.h>
#include "rgb.h"                 /* traz RGB_U32() */

/* cores */
#define PIX_HAPPY RGB_U32(0, 6, 0)   /* verde */
#define PIX_SAD   RGB_U32(6, 0, 0)   /* vermelho */
#define PIX_OFF   0x000000u

/* rostinho feliz (5×5) */
static const uint32_t frame_happy[25] = {
    PIX_OFF, PIX_HAPPY, PIX_OFF,  PIX_HAPPY, PIX_OFF,
    PIX_OFF, PIX_OFF,   PIX_OFF,  PIX_OFF,   PIX_OFF,
    PIX_HAPPY,PIX_OFF,  PIX_OFF,  PIX_OFF,   PIX_HAPPY,
    PIX_OFF, PIX_HAPPY, PIX_HAPPY,PIX_HAPPY, PIX_OFF,
    PIX_OFF, PIX_OFF,   PIX_OFF,  PIX_OFF,   PIX_OFF
};

/* rostinho triste */
static const uint32_t frame_sad[25] = {
    PIX_OFF, PIX_SAD,   PIX_OFF, PIX_SAD,    PIX_OFF,
    PIX_OFF, PIX_OFF,   PIX_OFF, PIX_OFF,    PIX_OFF,
    PIX_OFF, PIX_SAD,   PIX_SAD, PIX_SAD,    PIX_OFF,
    PIX_SAD, PIX_OFF,   PIX_OFF, PIX_OFF,    PIX_SAD,
    PIX_OFF, PIX_OFF,   PIX_OFF, PIX_OFF,    PIX_OFF
};
#endif
