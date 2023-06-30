#ifndef ANJIN_ECS_E_H
#define ANJIN_ECS_E_H

#include <stdint.h>

#define ECS_EIDXBITS 24
#define ECS_EGENBITS 8

typedef uint32_t ECS_e;    /* An entity */
typedef ECS_e    ECS_eint; /* An integer which can iterate through all entities */

int ECS_e_alive(ECS_e);
int ECS_e_set  (ECS_e, const char *, void *);

#define ECS_EIDX(e) (e & 0x00FFFFFF)
#define ECS_EGEN(e) ((e & 0xFF000000) >> ECS_EIDXBITS)

#define ECS_SET(e, c, ...) {struct c data = {__VA_ARGS__}; ECS_e_set(e, #c, &data);}

#endif