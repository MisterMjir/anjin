#ifndef ANJIN_SHADER_H
#define ANJIN_SHADER_H

unsigned int GL_shader_create    (const char *vpath, const char *fpath);
unsigned int GL_shader_create_str(const char *v, const char *f);
void         GL_shader_destroy   (unsigned int);

#endif