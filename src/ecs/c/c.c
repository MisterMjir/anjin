#include "c.h"
#include "log/log.h"
#include "../ecs.h"
#include <stdlib.h>

#define INITIAL_COUNT 8
#define GROWTH_RATE   1.618

int cm_alloc(struct ECS_cm *cm, unsigned int count)
{
  unsigned int *sizes;

  if (count <= cm->count) return -1;

  if (!(sizes = malloc(count * sizeof(*sizes)))) {
    LOG_msg("Out of memory\n");
    return -1;
  }

  memcpy(sizes, cm->sizes, cm->count * sizeof(*sizes));
  free(cm->sizes);
  cm->sizes = sizes;

  cm->alloc = count;

  return 0;
}

int ECS_cm_init(struct ECS_cm *cm)
{
  if (!(cm->sizes = malloc(INITIAL_COUNT * sizeof(*cm->sizes)))) {
    LOG_msg("out of memory\n");
    return -1;
  }
  cm->alloc = INITIAL_COUNT;
  cm->count = 2;

  *(cm->sizes + 0) = 0;
  *(cm->sizes + 1) = sizeof(struct ECS_c);

  return 0;
}

void ECS_cm_quit(struct ECS_cm *cm)
{
  free(cm->sizes);
}

int ECS_cm_add(struct ECS_cm *cm, unsigned int size)
{
  if (cm->alloc <= cm->count) {
    if (cm_alloc(cm, (unsigned int) (cm->count * GROWTH_RATE))) {
      LOG_msg("Out of memory\n");
      return -1;
    }
  }

  cm->sizes[cm->count++] = size;

  return 0;
}

int ECS_c_register(const char *n, unsigned int size)
{
  if (ECS_cm_add(&ECS_ctx->cm, size)) {
    LOG_msg("Could not register component %s\n", n);
    return -1;
  }
  if (ECS_cl_add(&ECS_ctx->cl, n, ECS_ctx->cm.count - 1)) {
    LOG_msg("Could not register component %s\n", n);
    return -1;
  }

  return 0;
}