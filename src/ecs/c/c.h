#ifndef ANJIN_ECS_C_H
#define ANJIN_ECS_C_H

#include "cl.h"
#include "../e/e.h"

struct ECS_c {
  ECS_e e;
};

/* Component manager */
struct ECS_cm {
  unsigned int  alloc;
  unsigned int  count;
  unsigned int *sizes;
};

int  ECS_cm_init(struct ECS_cm *);
void ECS_cm_quit(struct ECS_cm *);
int  ECS_cm_add (struct ECS_cm *, unsigned int);

int ECS_c_register(const char *, unsigned int);

#define ECS_C(c) ECS_c_register(#c, sizeof(struct c))

#endif