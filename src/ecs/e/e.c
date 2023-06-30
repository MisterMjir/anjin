#include "e.h"
#include "../t/tm.h"
#include "../type.h"
#include "../ecs.h"
#include "log/log.h"

int ECS_e_alive(ECS_e e)
{
  return ECS_EGEN(e) == ECS_ctx->em.gen[ECS_EIDX(e)];
}

int ECS_e_set(ECS_e e, const char *c, void *d)
{
  ECS_type      type;
  ECS_typei     typei;
  struct ECS_t *t;
  struct ECS_t *t_new;

  ECS_type_set(type, ECS_ctx->em.types[ECS_EIDX(e)]);
  typei = ECS_cl_get(&ECS_ctx->cl, c);

  if (!typei) {
    LOG_msg("Type %s doesn't exist\n", c);
    return -1;
  }

  t = ECS_tm_get(&ECS_ctx->tm, type);
  if (!ECS_TYPECHK(type, typei)) {
    ECS_TYPEADD(type, typei);
    t_new = ECS_tm_get(&ECS_ctx->tm, type);
    if (!t_new) {
      t_new = ECS_tm_add(&ECS_ctx->tm, type);
    }
    ECS_t_transfer(t, t_new, e);
    t = t_new;
  }
  ECS_t_set(t, e, typei, d);

  return 0;
}