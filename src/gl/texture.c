#include "texture.h"
#include <lodepng/lodepng.h>
#include "gl_util.h"

unsigned int GL_texture_create(const char *fpath)
{
  unsigned int   error;
  unsigned char *image;
  unsigned int   width, height;
  unsigned int   texture;

  error = lodepng_decode32_file(&image, &width, &height, fpath);
  if (error) {
    LOG_msg("Could not load image at %s\n", fpath);
    return 0;
  }

  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  /* I prefer GL_REPEAT but RECTANGLE can't have that */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  free(image);
  
  return texture;
}

void GL_texture_destroy(unsigned int t)
{
  glDeleteTextures(1, &t);
}