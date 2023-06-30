#include "cl.h"
#include <string.h>
#include "log/log.h"
#include <stdlib.h>

/* djb2 algorithm */
static unsigned int hash(const char *str)
{
  unsigned long hash;
  int c;

  hash = 5381;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;

  return hash;
}

int ECS_cl_init(struct ECS_cl *cl)
{
  for (unsigned int i = 0; i < ECS_MAX_COMPONENTS; ++i) {
    cl->t[i].key = NULL;
    cl->t[i].next = NULL;
  }

  ECS_cl_add(cl, "ECS_c", 1);

  return 0;
}

void ECS_cl_quit(struct ECS_cl *cl)
{
  struct ECS_cli *c; /* Current */
  struct ECS_cli *n; /* Next */

  for (unsigned int i = 0; i < ECS_MAX_COMPONENTS; ++i) {
    free(cl->t[i].key);

    n = cl->t[i].next;

    while (n) {
      c = n;
      n = n->next;
      free(c->key);
      free(c);
    }
  }
}

ECS_typei ECS_cl_get(struct ECS_cl *cl, const char *k)
{
  unsigned int i;
  struct ECS_cli *cli;

  i = hash(k) % ECS_MAX_COMPONENTS;
  cli = &cl->t[i];

  if (!cli->key) return 0;

  while (strcmp(k, cli->key)) {
    if (cli->next)
      cli = cli->next;
    else
      return 0;
  }

  return cli->value;
}

int ECS_cl_add(struct ECS_cl *cl, const char *k, ECS_typei i)
{
  unsigned int idx;
  struct ECS_cli *cli;

  idx = hash(k) % ECS_MAX_COMPONENTS;
  cli = &cl->t[idx];

  if (cl->t[idx].key) {
    while (cli->next) cli = cli->next;

    cli->next = malloc(sizeof(*cli->next));
    cli = cli->next;
  }

  cli->value = i;
  if (!(cli->key = malloc(strlen(k) + 1))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  strcpy(cli->key, k);

  return 0;
}