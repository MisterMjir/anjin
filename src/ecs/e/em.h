#ifndef ANJIN_ECS_EM_H
#define ANJIN_ECS_EM_H

#include "e.h"
#include "../type.h"

struct ECS_em {
  ECS_eint  alloc;
  ECS_eint  count;
  ECS_eint *gen;
  ECS_type *types;
  ECS_eint *idx;

  ECS_eint *fi;
  ECS_eint  fi_alloc;
  ECS_eint  fi_count;
};

int   ECS_em_init  (struct ECS_em *);
void  ECS_em_quit  (struct ECS_em *);
ECS_e ECS_e_create (void);
void  ECS_e_destroy(ECS_e);

#endif