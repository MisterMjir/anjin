#include "q.h"
#include "../ecs.h"
#include <stdlib.h>
#include "log/log.h"

static int table_match(struct ECS_q *q, struct ECS_t *t)
{
  for (ECS_typei i = 0; i < q->count; ++i) {
    switch (q->flags[i]) {
      case 1:
        if (!ECS_TYPECHK(t->type, q->types[i])) return -1;
        break;
      case 2:
        if (ECS_TYPECHK(t->type, q->types[i])) return -1;
        break;
      default:
        LOG_msg("Query error\n");
        return -1;
    }
  }

  return 0;
}

int ECS_q_init(struct ECS_q *q, const char *str)
{
  char           buffer[512];
  unsigned int   idx;
  unsigned int   flag_idx;
  const char    *str_start;
  struct ECS_qi *ins;

  q->count = 1;
  q->tables.table = NULL;
  q->tables.next = NULL;
  q->types = NULL;
  q->flags = NULL;

  /* First pass, find how many components are there */
  str_start = str;
  while (*str) {
    if (*str == '<') ++q->count;
    ++str;
  }

  if (!(q->types = malloc(q->count * sizeof(*q->types)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  if (!(q->flags = malloc(q->count * sizeof(*q->flags)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  q->flags[0] = 1;
  q->types[0] = 1;

  /* Second pass, figure out components */
  str = str_start;
  flag_idx = 1;
  #define BAD_QUERY {LOG_msg("Ill-formatted query\n"); ECS_q_quit(q); return -1;}
  while (*str) {
    if (*str != '<') BAD_QUERY;
    ++str;
    switch (*str) {
      case '|':
        q->flags[flag_idx] = 1;
        break;
      case '~':
        q->flags[flag_idx] = 2;
        break;
      default:
        BAD_QUERY;
    }
    ++str;
    idx = 0;
    while (*str != '>') {
      if (!*str) BAD_QUERY;
      buffer[idx++] = *str++;
    }
    buffer[idx] = '\0';
    if (!(q->types[flag_idx++] = ECS_cl_get(&ECS_ctx->cl, buffer))) BAD_QUERY;
    ++str;
  }
  #undef BAD_QUERY

  /* Do the actual querying */
  ins = &q->tables;
  for (unsigned long i = 0; i < ECS_ctx->tm.count; ++i) {
    if (!table_match(q, &ECS_ctx->tm.tables[i])) {
      if (ins->next) ins = ins->next;
      ins->table = &ECS_ctx->tm.tables[i];
      if (!(ins->next = malloc(sizeof(*ins->next)))) {
        LOG_msg("Out of memory\n");
        return -1;
      }
    }
  }

  free(ins->next);
  ins->next = NULL;

  return 0;
}

void ECS_q_quit(struct ECS_q *q)
{
  struct ECS_qi *prev;
  struct ECS_qi *cur;

  cur = q->tables.next;
  while (cur) {
    prev = cur;
    cur = cur->next;
    free(prev);
  }
  free(q->types);
  free(q->flags);
}

void ECS_iterate(struct ECS_q *q, void (*cb)(struct ECS_qd *, ECS_eint))
{
  struct ECS_qi *cur;
  struct ECS_qd  qd;

  if (q->tables.table == NULL) return;

  cur = &q->tables;
  while (cur) {
    qd.table = cur->table;
    cb(&qd, cur->table->count);
    cur = cur->next;
  }
}

void * ECS_cget(struct ECS_qd *qd, const char *comp)
{
  ECS_typei idx;

  if (!(idx = ECS_cl_get(&ECS_ctx->cl, comp))) {
    LOG_msg("%s is not a registered component\n", comp);
    return NULL;
  }

  idx = ECS_t_type_idx(qd->table, idx);

  return (char *) qd->table->buffer + qd->table->offsets[idx] * qd->table->alloc;
}