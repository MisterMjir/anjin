#include "txt.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "log/log.h"
#include <cglm/cglm.h>
#include "gl/gl_util.h"

static const char *vshdr =
  "#version 300 es\n"
  "precision mediump float;"
  "layout (location = 0) in vec4 vertex;" // <vec2 pos, vec2 tex>
  "uniform mat4 projection;"
  "out vec2 TexCoords;"
  "void main()"
  "{"
    "gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);"
    "TexCoords = vertex.zw;"
  "}";

static const char *fshdr =
  "#version 300 es\n"
  "precision mediump float;"
  "in vec2 TexCoords;"
  "uniform sampler2D text;"
  "uniform vec3 textColor;"
  "out vec4 color;"
  "void main()"
  "{"    
    "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);"
    "color = vec4(textColor, 1.0) * sampled;"
  "}";

static FT_Library ft;
static FT_Face face;

struct character {
  unsigned int txt;
  ivec2        size;
  ivec2        bearing;
  unsigned int advance;
};

static unsigned int shader;
static unsigned int VAO;
static unsigned int VBO;

struct character characters[128];

int TXT_init(void)
{
  if (FT_Init_FreeType(&ft)) {
    LOG_msg("Could not initialize free type\n");
    return -1;
  }

  if (FT_New_Face(ft, "res/core/Outfit.ttf", 0, &face)) {
    LOG_msg("Failed to load font\n");
    return -1;
  }
  FT_Set_Pixel_Sizes(face, 0, 24);

  shader = GL_shader_create_str(vshdr, fshdr);
  glUseProgram(shader);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  for (unsigned char c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      LOG_msg("Failed to load glpyh\n");
      continue;
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    /* GL_RED is invalid in webgl */
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int vec[2];
    characters[c].txt = texture;
    vec[0] = face->glyph->bitmap.width;
    vec[1] = face->glyph->bitmap.rows;
    glm_ivec2(vec, characters[c].size);
    vec[0] = face->glyph->bitmap_left;
    vec[1] = face->glyph->bitmap_top;
    glm_ivec2(vec, characters[c].bearing);
    characters[c].advance = face->glyph->advance.x;
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 0;
}

void TXT_quit(void)
{
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

/**
 * TXT_draw
 *
 * Current setup for +y going down, should add something to
 * accomodate +y going up but not implemented. Also I think
 * the y coordinate is the baseline, not the top left
 */
void TXT_draw(const char *str, float x, float y, float scale)
{
  glUseProgram(shader);
  glUniform3f(glGetUniformLocation(shader, "textColor"), 1.0f, 1.0f, 1.0f);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  while (*str) {
    struct character c = characters[*str];

    float xpos = x + c.bearing[0] * scale;
    //float ypos = y - (c.size[1] - c.bearing[1]) * scale;
    float ypos = y - c.bearing[1] * scale;
    float w = c.size[0] * scale;
    float h = c.size[1] * scale;

    // If text is upside down flip ypos + h and ypos + 0
    float vertices[6][4] = {
      {xpos,     ypos, 0.0f, 0.0f},
      {xpos,     ypos + h,     0.0f, 1.0f},
      {xpos + w, ypos + h,     1.0f, 1.0f},
      {xpos,     ypos, 0.0f, 0.0f},
      {xpos + w, ypos + h,     1.0f, 1.0f},
      {xpos + w, ypos, 1.0f, 0.0f}
    };

    glBindTexture(GL_TEXTURE_2D, c.txt);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    x += (c.advance >> 6) * scale;

    ++str;
  }
}

void TXT_projection(float l, float r, float b, float t, float back, float front)
{
  mat4 projection;

  glUseProgram(shader);

  glm_ortho(l, r, b, t, back, front, projection);
  glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, (float *) projection);
}