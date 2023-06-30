#include "ecs.h"

struct ECS_ctx *ECS_ctx;

int ECS_init(struct ECS_ctx *ctx)
{
  ECS_cm_init(&ctx->cm);
  ECS_cl_init(&ctx->cl);
  ECS_em_init(&ctx->em);
  ECS_tm_init(&ctx->tm);
  return 0;
}

void ECS_quit(struct ECS_ctx *ctx)
{
  ECS_tm_quit(&ctx->tm);
  ECS_em_quit(&ctx->em);
  ECS_cl_quit(&ctx->cl);
  ECS_cm_quit(&ctx->cm);
}

void ECS_set(struct ECS_ctx *ctx)
{
  ECS_ctx = ctx;
}