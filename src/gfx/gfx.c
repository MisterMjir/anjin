#include "gfx.h"
#include "gl/gl_util.h"
#include <cglm/cglm.h>

static const char *vshdr =
	"#version 300 es\n"
  "precision mediump float;"
  "layout (location = 0) in vec3 aPos;"
  "layout (location = 1) in vec2 aTexCoord;"
  "uniform mat4 projection;"
  "uniform vec2 scale;"
  "uniform vec2 shift;"
  "out vec2 TexCoord;"
  "void main()"
  "{"
	  "gl_Position = projection * vec4(shift.x + aPos.x * scale.x, shift.y + aPos.y * scale.y, aPos.z, 1.0);"
	  "TexCoord = vec2(aTexCoord.x, aTexCoord.y);"
  "}";

static const char *fshdr =
  "#version 300 es\n"
  "precision mediump float;"
  "in vec2 TexCoord;"
  "uniform sampler2D texture1;"
  "out vec4 FragColor;"
  "void main()"
  "{"
	  "FragColor = texture(texture1, TexCoord);"
  "}";

static float vertices[] = {
   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
   0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
   0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
};

static unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};

static unsigned int VBO, VAO, EBO;
static unsigned int texture;
static unsigned int shader;

int GFX_init(void)
{
  shader = GL_shader_create_str(vshdr, fshdr);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return 0;
}

void GFX_quit(void)
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  GL_shader_destroy(shader);
}

void GFX_projection(float l, float r, float b, float t, float back, float front)
{
  mat4 projection;

  glUseProgram(shader);

  glm_ortho(l, r, b, t, back, front, projection);
  glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (float *) projection);
}

void GFX_draw_img(unsigned int t, float x, float y, float w, float h)
{
  glBindTexture(GL_TEXTURE_2D, t);

  glUseProgram(shader);

  glUniform2f(glGetUniformLocation(shader, "shift"), x, y);
  glUniform2f(glGetUniformLocation(shader, "scale"), w, h);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}