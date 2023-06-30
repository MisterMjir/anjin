#ifndef ANJIN_ECS_H
#define ANJIN_ECS_H

#include "c/c.h"
#include "e/em.h"
#include "t/tm.h"
#include "q/q.h"

struct ECS_ctx {
  struct ECS_cm cm;
  struct ECS_cl cl;
  struct ECS_em em;
  struct ECS_tm tm;
};

extern struct ECS_ctx *ECS_ctx; /* Current context */

int  ECS_init(struct ECS_ctx *);
void ECS_quit(struct ECS_ctx *);
void ECS_set (struct ECS_ctx *);

#endif