#include "type.h"

void ECS_type_init(ECS_type t)
{
  t[0] = 2;

  for (ECS_typei i = 1; i < ECS_TYPEINTS; ++i)
    t[i] = 0;
}

int ECS_type_num(ECS_type t)
{
  return 0;
}

int ECS_type_cmp(ECS_type a, ECS_type b)
{
  for (ECS_typei i = 0; i < ECS_TYPEINTS; ++i) {
    if (a[i] != b[i]) return 0;
  }

  return 1;
}

void ECS_type_set(ECS_type a, ECS_type b)
{
  for (ECS_typei i = 0; i < ECS_TYPEINTS; ++i) {
    a[i] = b[i];
  }
}