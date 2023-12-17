#include "shader.h"
#include "gll.h"
#include <stdio.h>
#include "log/log.h"
#include <stdlib.h>

#define LOG_BUFFER 512

static int fts(const char *fpath, char **str)
{
  FILE  *file;
  size_t size;
  
  if (!(file = fopen(fpath, "r"))) {
    LOG_msg("Could not open file: %s\n", fpath);
    return -1;
  }

  size = 1; /* Account for '\0' */
  while (fgetc(file) != EOF) {
    ++size;
  }
  if (!(*str = malloc(size))) {
    LOG_msg("Out of memory\n");
    return -1;
  }
  if (fseek(file, 0, SEEK_SET)) {
    LOG_msg("Could not seek in file\n");
    return -1;
  }

  if (fread(*str, sizeof(char), size - 1, file) != size - 1) {
    LOG_msg("Could not read file\n");
    return -1;
  }
  (*str)[size - 1] = '\0';

  fclose(file);

  return 0;
}

static unsigned int compile_shader(const char *fpath, GLenum type)
{
  char        *str;
  unsigned int s;
  char         info[LOG_BUFFER];
  int          success;

  if (fts(fpath, &str)) {
    LOG_msg("Could not convert shader to string\n");
    return -1;
  }

  s = glCreateShader(type);
  glShaderSource(s, 1, &str, NULL);
  glCompileShader(s);

  glGetShaderiv(s, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(s, LOG_BUFFER, NULL, info);
    LOG_msg("Error compiling shader: %s\n", info);
    free(str);
    return 0;
  }

  free(str);
  return s;
}

unsigned int GL_shader_create(const char *vpath, const char *fpath)
{
  unsigned int vs;
  unsigned int fs;
  unsigned int s;
  int          success;
  char         info[LOG_BUFFER];

  if (!(vs = compile_shader(vpath, GL_VERTEX_SHADER))) {
    return 0;
  }

  if (!(fs = compile_shader(fpath, GL_FRAGMENT_SHADER))) {
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

void GL_shader_destroy(unsigned int s)
{
  glDeleteShader(s);
}