#ifndef ANJIN_ECS_CL_H
#define ANJIN_ECS_CL_H

#include "../type.h"

/* This just needs to be an estimate, it is how many indices the lookup table gets */
#define ECS_MAX_COMPONENTS 128

/* Component look-up */
struct ECS_cli {
  char           *key;
  ECS_typei       value;
  struct ECS_cli *next;
};

struct ECS_cl {
  struct ECS_cli t[ECS_MAX_COMPONENTS];
};

int       ECS_cl_init(struct ECS_cl *);
void      ECS_cl_quit(struct ECS_cl *);
int       ECS_cl_add (struct ECS_cl *cl, const char *k, ECS_typei i);
ECS_typei ECS_cl_get (struct ECS_cl *cl, const char *k);

#endif