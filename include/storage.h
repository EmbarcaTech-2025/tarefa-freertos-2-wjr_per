#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdbool.h>
#include "pet.h"

/* “assinatura” de um snapshot salvo na flash */
typedef struct
{
    uint32_t     magic;   /* 0x53544F52 = "STOR" */
    pet_state_t  state;   /* struct com idade/fun/hunger/sleep */
    uint32_t     crc;     /* CRC-32 do campo state              */
} pet_snapshot_t;

#define STORAGE_MAGIC 0x53544F52u   /* “STOR” */

/* API --------------------------------------------------------------------- */
void  storage_init(void);                      /* não bloqueia               */
bool  storage_load(pet_state_t *dst);          /* true  → válido             */
bool  storage_save(const pet_snapshot_t *src); /* false → erro de gravação   */

#endif /* STORAGE_H_ */
