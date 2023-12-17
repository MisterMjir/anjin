#include "em.h"
#include "log/log.h"
#include "../ecs.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_COUNT 8
#define GROWTH_RATE   1.618

#define MALLOC(var, count) \
  if (!(em->var = malloc(count * sizeof(*em->var)))) { \
    LOG_msg("Out of memory\n"); \
    return -1; \
  }

#define CALLOC(var, count) \
  if (!(em->var = calloc(count, sizeof(*em->var)))) { \
    LOG_msg("Out of memory"); \
    return -1; \
  }

#define MOVE(src, dst, count) \
  memcpy(src, dst, sizeof(*dst) * count); \
  free(dst); \
  dst = src;

#define COPYM(var, count, num) \
  void *ECS_new_##var; \
  if (!(ECS_new_##var = malloc(count * sizeof(*em->var)))) { \
    LOG_msg("Cannot allocate entity manager: Out of memory"); \
    return -2; \
  } \
  MOVE(ECS_new_##var, em->var, em->num);

#define COPYC(var, count, num) \
  void *ECS_new_##var; \
  if (!(ECS_new_##var = calloc(count, sizeof(*em->var)))) { \
    LOG_msg("Cannot allocate entity manager: Out of memory"); \
    return -2; \
  } \
  MOVE(ECS_new_##var, em->var, em->num);

int ECS_em_init(struct ECS_em *em)
{
  CALLOC(gen,   INITIAL_COUNT);
  MALLOC(types, INITIAL_COUNT);
  MALLOC(idx,   INITIAL_COUNT);
  em->alloc = INITIAL_COUNT;
  em->count = 1;

  MALLOC(fi, INITIAL_COUNT);
  em->fi_alloc = INITIAL_COUNT;
  em->fi_count = 0;

  return 0;
}

void ECS_em_quit(struct ECS_em *em)
{
  free(em->gen);
  free(em->types);
  free(em->idx);
  free(em->fi);
}

int ECS_em_alloc(struct ECS_em *em, ECS_eint count)
{
  if (count <= em->alloc) return -1;

  COPYC(gen,   count, count);
  COPYM(types, count, count);
  COPYM(idx,   count, count);

  em->alloc = count;

  return 0;
}

int ECS_em_fi_alloc(struct ECS_em *em, ECS_eint count)
{
  if (count <= em->fi_alloc) return -1;

  COPYM(fi, count, fi_count);

  em->alloc = count;

  return 0;
}

/*
 * ECS_e_create
 *
 * Assigns an entity with an id, initializes its type,
 * and adds it to a table
 */
ECS_e ECS_e_create(void)
{
  ECS_e e;
  struct ECS_em *em;
  struct ECS_tm *tm;
  
  em = &ECS_ctx->em;
  tm = &ECS_ctx->tm;

  if (em->count == (1 << ECS_EIDXBITS) - 1) {
    LOG_msg("Can't create a new entity\n");
    return 0;
  }

  /* Try to reuse an index */
  if (em->fi_count > 0) {
    --em->fi_count;
    e = em->gen[em->fi[em->fi_count]];
    e <<= ECS_EIDXBITS;
    e |= em->fi[em->fi_count];
  }
  else {
    if (em->count <= em->alloc) {
      if (em->count == em->alloc) {
        if (ECS_em_alloc(em, (ECS_eint) (em->alloc * GROWTH_RATE))) {
          LOG_msg("Cannot create entity\n");
          return 0;
        }
      }
    }

    e = em->count++; /* Pre-increment skips idx 1 */
  }

  ECS_t_add(tm->tables, e);

  return e;
}

void ECS_e_destroy(ECS_e e)
{
  struct ECS_em *em;
  struct ECS_t *t;

  if (!ECS_e_alive(e)) return;

  em = &ECS_ctx->em;

  if (em->fi_count == em->fi_alloc) {
    if (ECS_em_fi_alloc(em, em->fi_alloc * GROWTH_RATE)) {
      LOG_msg("Out of memory\n");
    }
  }

  t = ECS_tm_get(&ECS_ctx->tm, ECS_ctx->em.types[ECS_EIDX(e)]);
  ECS_t_remove(t, e);

  ++em->gen[ECS_EIDX(e)];
  em->fi[em->fi_count++] = ECS_EIDX(e);
}