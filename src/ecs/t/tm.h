#ifndef ANJIN_ECS_TM_H
#define ANJIN_ECS_TM_H

#include "t.h"
#include "../type.h"

struct ECS_tm {
  unsigned long alloc;
  unsigned long count;
  struct ECS_t *tables;
};

int            ECS_tm_init(struct ECS_tm *);
void           ECS_tm_quit(struct ECS_tm *);
struct ECS_t * ECS_tm_get (struct ECS_tm *, ECS_type);
struct ECS_t * ECS_tm_add (struct ECS_tm *, ECS_type);

#endif