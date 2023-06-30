#ifndef ANJIN_GL_UTIL_H
#define ANJIN_GL_UTIL_H

#include "gll.h"
#include "shader.h"
#include "texture.h"

int  GL_init       (void);
int  GL_window_init(struct CORE_window *w);
int  GL_set        (struct CORE_window *w);
void GL_unset      (struct CORE_window *w);

#endif