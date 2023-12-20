#include "shader.h"
#include "gll.h"
#include <stdio.h>
#include "log/log.h"
#include <stdlib.h>

#define LOG_BUFFER 512

static char * fts(const char *fpath)
{
  FILE  *file;
  size_t size;
  char  *str;
  
  if (!(file = fopen(fpath, "r"))) {
    LOG_msg("Could not open file: %s\n", fpath);
    return NULL;
  }

  size = 1; /* Account for '\0' */
  while (fgetc(file) != EOF) {
    ++size;
  }
  if (!(str = malloc(size))) {
    LOG_msg("Out of memory\n");
    return NULL;
  }
  if (fseek(file, 0, SEEK_SET)) {
    LOG_msg("Could not seek in file\n");
    return NULL;
  }

  if (fread(str, sizeof(char), size - 1, file) != size - 1) {
    LOG_msg("Could not read file\n");
    return NULL;
  }
  str[size - 1] = '\0';

  fclose(file);

  return str;
}

static unsigned int compile_shader(const char *str, GLenum type)
{
  unsigned int s;
  char         info[LOG_BUFFER];
  int          success;

  s = glCreateShader(type);
  glShaderSource(s, 1, &str, NULL);
  glCompileShader(s);

  glGetShaderiv(s, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(s, LOG_BUFFER, NULL, info);
    LOG_msg("Error compiling shader: %s\n", info);
    return 0;
  }

  return s;
}

/**
 * GL_shader_create_str
 */
unsigned int GL_shader_create_str(const char *v, const char *f)
{
  unsigned int vs;
  unsigned int fs;
  unsigned int s;
  int          success;
  char         info[LOG_BUFFER];

  if (!(vs = compile_shader(v, GL_VERTEX_SHADER))) {
    return 0;
  }

  if (!(fs = compile_shader(f, GL_FRAGMENT_SHADER))) {
    return 0;
  }

  s = glCreateProgram();
  glAttachShader(s, vs);
  glAttachShader(s, fs);
  glLinkProgram(s);

  glGetProgramiv(s, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(s, LOG_BUFFER, NULL, info);
    LOG_msg("Error while linking shader: %s\n", info);
    return 0;
  }

  glDeleteShader(vs);
  glDeleteShader(fs);

  return s;
}

unsigned int GL_shader_create(const char *vpath, const char *fpath)
{
  char        *vstr;
  char        *fstr;
  unsigned int s;

  if (!(vstr = fts(vpath))) {
    LOG_msg("Could not convert vertex shader to string\n");
    return 0;
  }

  if (!(fstr = fts(fpath))) {
    LOG_msg("Could not convert fragmemt shader to string\n");
    return 0;
  }

  s = GL_shader_create_str(vstr, fstr);

  free(vstr);
  free(fstr);

  return s;
}

void GL_shader_destroy(unsigned int s)
{
  glDeleteShader(s);
}