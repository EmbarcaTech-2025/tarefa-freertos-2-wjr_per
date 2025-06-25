#ifndef PET_H
#define PET_H

#include <stdint.h>
#include <stdbool.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"

/* ------------------------------------------------------------------ */
/* 𝟙  Event bits (broadcast)                                           */
#define EVT_FEED   (1 << 0)
#define EVT_PLAY   (1 << 1)
#define EVT_SLEEP  (1 << 2)
#define EVT_WAKE   (1 << 3)
#define EVT_TICK   (1 << 4)
#define EVT_SNAKE_START  (1<<5)

/* 𝟚  Estado persistente                                               */
typedef struct {
    uint32_t age_sec;
    uint8_t  hunger;      // 0–10
    uint8_t  fun;         // 0–10
    bool     sleeping;
} pet_state_t;

/* 𝟛  Handles globais                                                  */
extern pet_state_t       gPet;
extern EventGroupHandle_t egPet;
extern SemaphoreHandle_t  mtxPet;

#endif /* PET_H */
