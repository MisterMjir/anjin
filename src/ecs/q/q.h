#ifndef ANJIN_ECS_Q_H
#define ANJIN_ECS_Q_H

#include "../type.h"
#include "../t/t.h"

struct ECS_qi {
  struct ECS_t  *table;
  struct ECS_qi *next;
};

struct ECS_q {
  unsigned long count;
  struct ECS_qi tables;
  ECS_typei    *types;
  int          *flags;
};

struct ECS_qd {
  struct ECS_t *table;
};

int    ECS_q_init (struct ECS_q *, const char *);
void   ECS_q_quit (struct ECS_q *);
void   ECS_iterate(struct ECS_q *, void (*)(struct ECS_qd *, ECS_eint));
void * ECS_cget   (struct ECS_qd *, const char *);

#define ECS_ITER(d, c) (struct c *) ECS_cget(d, #c);

#endif