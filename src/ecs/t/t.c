#include "t.h"
#include "log/log.h"
#include "ecs/ecs.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_COUNT 8
#define GROWTH_RATE   1.618

static int t_init_type(struct ECS_t *t, ECS_type type)
{
  ECS_typei idx, ins;

  ECS_type_set(t->type, type);

  t->type_count = 0;

  for (ECS_typei i = 0; i < ECS_TYPEINTS; ++i) {
    for (char j = 0; j < 8; ++j) {
      t->type_count += ((type[i] >> j) & 1);
    }
  }

  if (!(t->types = malloc(t->type_count * sizeof(*t->types)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  if (!(t->sizes = malloc(t->type_count * sizeof(*t->sizes)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  if (!(t->offsets = malloc(t->type_count * sizeof(*t->sizes)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  idx = 0;
  ins = 0;
  t->size = 0;
  t->offsets[0] = 0;
  for (ECS_typei i = 0; i < ECS_TYPEINTS; ++i) {
    for (char j = 0; j < 8; ++j) {
      if ((type[i] >> j) & 1) {
        t->types[ins] = idx;
        t->sizes[ins] = ECS_ctx->cm.sizes[idx];
        if (ins) {
          t->offsets[ins] = t->size;
        }
        t->size += ECS_ctx->cm.sizes[idx];
        ++ins;
      }
      ++idx;
    }
  }

  return 0;
}

int ECS_t_init(struct ECS_t *t, ECS_type type)
{
  if (t_init_type(t, type)) {
    LOG_msg("Could not init table\n");
    return -1;
  }

  if (!(t->buffer = malloc(INITIAL_COUNT * t->size))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  t->count = 1;
  t->alloc = INITIAL_COUNT;

  return 0;
}

void ECS_t_quit(struct ECS_t *t)
{
  free(t->buffer);
  free(t->types);
  free(t->sizes);
  free(t->offsets);
}

static int t_alloc(struct ECS_t *t, ECS_eint count)
{
  void *buffer;

  if (count <= t->alloc) return -1;

  if (!(buffer = malloc(count * t->size))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  for (ECS_typei i = 0; i < t->type_count; ++i) {
    memcpy(
      (char *) buffer + t->offsets[i] * count,
      (char *) t->buffer + t->offsets[i] * t->alloc,
      t->count * t->sizes[i]
    );
  }

  free(t->buffer);
  t->buffer = buffer;
  t->alloc = count;

  return 0;
}

static int t_add_chk(struct ECS_t* t)
{
  if (t->alloc <= t->count) {
    if (t_alloc(t, t->count * GROWTH_RATE)) {
      LOG_msg("Out of memory\n");
      return -1;
    }
  }

  return 0;
}

int ECS_t_add(struct ECS_t *t, ECS_e e)
{
  if (t_add_chk(t)) return -1;

  ECS_ctx->em.idx[ECS_EIDX(e)] = t->count;
  ECS_type_set(ECS_ctx->em.types[ECS_EIDX(e)], t->type);
  ((ECS_eint *) t->buffer)[t->count] = e;

  ++t->count;

  return 0;
}

ECS_typei ECS_t_type_idx(struct ECS_t *t, ECS_typei type)
{
  for (ECS_typei i = 0; i < t->type_count; ++i) {
    if (t->types[i] == type)
      return i;
  }

  return -1;
}

void ECS_t_update_type(struct ECS_t *t, ECS_e e, ECS_typei type, void *data)
{
  ECS_typei tidx;
  
  tidx = ECS_t_type_idx(t, type);
  memcpy(
    (char *) t->buffer + t->offsets[tidx] * t->alloc + t->sizes[tidx] * t->count,
    data,
    t->sizes[tidx]
  );
}

void ECS_t_remove(struct ECS_t *t, ECS_e e)
{
  ECS_eint idx;

  idx = ECS_ctx->em.idx[ECS_EIDX(e)];

  for (ECS_typei i = 0; i < t->type_count; ++i) {
    memcpy(
      (char *) t->buffer + t->offsets[i] * t->alloc + t->sizes[i] * idx,
      (char *) t->buffer + t->offsets[i] * t->alloc + t->sizes[i] * (t->count - 1),
      t->sizes[i]
    );
  }

  e = ((ECS_e *) t->buffer)[idx];
  ECS_ctx->em.idx[ECS_EIDX(e)] = idx;

  --t->count;
}

int ECS_t_transfer(struct ECS_t *a, struct ECS_t *b, ECS_e e)
{
  ECS_eint  old_idx;
  ECS_eint  new_idx;
  ECS_typei type_idx;

  if (t_add_chk(b)) return -1;

  old_idx = ECS_ctx->em.idx[ECS_EIDX(e)];
  new_idx = b->count;

  for (ECS_typei i = 0; i < a->type_count; ++i) {
    //if (a->types[i] == 1) continue; /* Skip the ECS_c component */
    if ((type_idx = ECS_t_type_idx(b, a->types[i])) != (ECS_typei) -1) {
      memcpy(
        (char *) b->buffer + b->offsets[type_idx] * b->alloc + b->sizes[type_idx] * new_idx,
        (char *) a->buffer + a->offsets[i]        * a->alloc + a->sizes[i]        * old_idx,
        a->sizes[i]
      );
    }
  }

  ECS_t_remove(a, e);
  ECS_t_add(b, e);

  return 0;
}

int ECS_t_set(struct ECS_t *t, ECS_e e, ECS_typei ti, void *d)
{
  ECS_typei tidx;
  ECS_eint  idx;

  if (!(tidx = ECS_t_type_idx(t, ti))) {
    return -1;
  }

  idx = ECS_ctx->em.idx[ECS_EIDX(e)];

  memcpy(
    (char *) t->buffer + t->offsets[tidx] * t->alloc + t->sizes[tidx] * idx,
    d,
    t->sizes[tidx]
  );

  return 0;
}