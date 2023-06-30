#ifndef ANJIN_ECS_T_H
#define ANJIN_ECS_T_H

#include "../e/e.h"
#include "../type.h"

struct ECS_t {
  ECS_eint      alloc;
  ECS_eint      count;
  void         *buffer;
  ECS_type      type;
  ECS_typei    *types;
  ECS_typei     type_count;
  unsigned int *sizes;
  unsigned int  size;
  unsigned int *offsets;
};

int  ECS_t_init    (struct ECS_t *, ECS_type);
void ECS_t_quit    (struct ECS_t *);
int  ECS_t_add     (struct ECS_t *, ECS_e);
void ECS_t_remove  (struct ECS_t *, ECS_e);
int  ECS_t_transfer(struct ECS_t *, struct ECS_t *, ECS_e);
int  ECS_t_set     (struct ECS_t *, ECS_e, ECS_typei, void *);

ECS_typei ECS_t_type_idx(struct ECS_t *t, ECS_typei type);


#endif