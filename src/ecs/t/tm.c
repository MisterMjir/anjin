#include "tm.h"
#include "../c/c.h"
#include <stdlib.h>

#define INITIAL_COUNT 8
#define GROWTH_RATE   1.618

static int first_table(struct ECS_tm *tm)
{
  #define TABLE (tm->tables[0])
  #define SIZE sizeof(struct ECS_c)
  TABLE.alloc = INITIAL_COUNT;
  TABLE.count = 1;
  if (!(TABLE.buffer = malloc(INITIAL_COUNT * SIZE))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  ((ECS_e *) TABLE.buffer)[0] = 0;
  ECS_type_init(TABLE.type);
  if (!(TABLE.types = malloc(sizeof(*TABLE.types)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  *TABLE.types = 1;
  TABLE.type_count = 1;
  if (!(TABLE.sizes = malloc(sizeof(*TABLE.sizes)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  *TABLE.sizes = SIZE;
  TABLE.size = SIZE;
  if (!(TABLE.offsets = malloc(sizeof(*TABLE.offsets)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  *TABLE.offsets = 0;
  #undef SIZE
  #undef TABLE

  ++tm->count;

  return 0;
}

int ECS_tm_init(struct ECS_tm *tm)
{
  if (!(tm->tables = malloc(INITIAL_COUNT * sizeof(*tm->tables)))) {
    LOG_msg("out of memory\n");
    return -1;
  }
  tm->alloc = INITIAL_COUNT;
  tm->count = 0;

  first_table(tm);

  return 0;
}

void ECS_tm_quit(struct ECS_tm *tm)
{
  free(tm->tables);
}

int ECS_tm_alloc(struct ECS_tm *tm, unsigned long count)
{
  struct ECS_t *tables;

  if (count <= tm->count) return -1;

  if (!(tables = malloc(count * sizeof(*tables)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  memcpy(tables, tm->tables, tm->count * sizeof(*tables));
  free(tm->tables);
  tm->tables = tables;

  tm->alloc = count;

  return 0;
}

struct ECS_t * ECS_tm_add(struct ECS_tm *tm, ECS_type t)
{
  if (tm->alloc <= tm->count) {
    if (cm_alloc(tm, (unsigned long) (tm->count * GROWTH_RATE))) {
      LOG_msg("Out of memory\n");
      return NULL;
    }
  }

  ECS_t_init(&tm->tables[tm->count], t);

  return &tm->tables[tm->count++];
}

struct ECS_t * ECS_tm_get(struct ECS_tm *tm, ECS_type type)
{
  for (unsigned long i = 0; i < tm->count; ++i) {
    if (ECS_type_cmp(type, tm->tables[i].type)) {
      return &tm->tables[i];
    }
  }

  return NULL;
}