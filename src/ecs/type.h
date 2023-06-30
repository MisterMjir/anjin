#ifndef ANJIN_ECS_TYPE_H
#define ANJIN_ECS_TYPE_H

#include <stdint.h>

#define ECS_TYPEINTS 4

typedef uint32_t     ECS_type[ECS_TYPEINTS]; /* A type */
typedef unsigned int ECS_typei;              /* An integer which is able to iterate through all types */

void ECS_type_init(ECS_type);
int  ECS_type_num (ECS_type);
int  ECS_type_cmp (ECS_type, ECS_type);
void ECS_type_set (ECS_type, ECS_type);

#define ECS_TYPEIDX(i) (i / sizeof(ECS_type) / ECS_TYPEINTS * 8)
#define ECS_TYPEBIT(i) (i % (sizeof(ECS_type) / ECS_TYPEINTS * 8))
#define ECS_TYPECHK(t, i) (t[ECS_TYPEIDX(i)] & (1 << ECS_TYPEBIT(i)))
#define ECS_TYPEADD(t, i) t[ECS_TYPEIDX(i)] |= (1 << ECS_TYPEBIT(i))
#define ECS_TYPEDEL(t, i) t[ECS_TYPEIDX(i)] &= ~(1 << ECS_TYPEBIT(i))

#endif