#ifndef ANJIN_GLL_H
#define ANJIN_GLL_H

#ifdef __EMSCRIPTEN__
  #include <GL/gl.h>
  #include <GLES3/gl3.h>
#elif _WIN32
  #include <windows.h>
  #define GLDECL WINAPI
  #include <GL/gl.h>
  #include <GL/glext.h>
#else
  #error Platform not supported (GL)
#endif

int GL_gll(void);

/* For some reason linux and windows are inconsistent with glActiveTexture */
#ifdef _WIN32
  #define GL_PROCS_WINDOWS \
    GLFN(void, glActiveTexture, GLenum)
#else
  #define GL_PROCS_WINDOWS
#endif
/* Declare the functions */
#define GL_PROCS \
  GL_PROCS_WINDOWS \
  GLFN(void,            glAttachShader,            GLuint, GLuint) \
  GLFN(void,            glBindBuffer,              GLenum, GLuint) \
  GLFN(void,            glBindVertexArray,         GLuint) \
  GLFN(void,            glBufferData,              GLenum, GLsizeiptr, const GLvoid *, GLenum) \
  GLFN(void,            glBufferSubData,           GLenum, GLintptr, GLsizeiptr, const GLvoid *) \
  GLFN(void,            glCompileShader,           GLuint) \
  GLFN(GLuint,          glCreateShader,            GLenum) \
  GLFN(GLuint,          glCreateProgram,           void) \
  GLFN(void,            glDebugMessageCallback,    GLDEBUGPROC, void *) \
  GLFN(void,            glDeleteBuffers,           GLsizei, const GLuint *) \
  GLFN(void,            glDeleteShader,            GLuint) \
  GLFN(void,            glDeleteVertexArrays,      GLsizei, const GLuint *) \
  GLFN(void,            glEnableVertexAttribArray, GLuint) \
  GLFN(void,            glGenBuffers,              GLsizei, GLuint *) \
  GLFN(void,            glGenVertexArrays,         GLsizei, GLuint *) \
  GLFN(void,            glGetProgramInfoLog,       GLuint, GLsizei, GLsizei *, GLchar *) \
  GLFN(void,            glGetProgramiv,            GLuint, GLenum, GLint *) \
  GLFN(void,            glGetShaderiv,             GLuint, GLenum, GLint *) \
  GLFN(void,            glGetShaderInfoLog,        GLuint, GLsizei, GLsizei *, GLchar *) \
  GLFN(const GLubyte *, glGetStringi,              GLenum, GLuint) \
  GLFN(GLint,           glGetUniformLocation,      GLuint, const GLchar *) \
  GLFN(void,            glLinkProgram,             GLuint) \
  GLFN(void *,          glMapBufferRange,          GLenum, GLintptr, GLsizeiptr, GLbitfield) \
  GLFN(void,            glShaderSource,            GLuint, GLsizei, const GLchar **, const GLint *) \
  GLFN(void,            glUniform1f,               GLint, GLfloat) \
  GLFN(void,            glUniform2f,               GLint, GLfloat, GLfloat) \
  GLFN(void,            glUniform3f,               GLint, GLfloat, GLfloat, GLfloat) \
  GLFN(void,            glUniform4f,               GLint, GLfloat, GLfloat, GLfloat, GLfloat) \
  GLFN(void,            glUniformMatrix4fv,        GLint, GLsizei, GLboolean, const GLfloat *) \
  GLFN(GLboolean,       glUnmapBuffer,             GLenum) \
  GLFN(void,            glUseProgram,              GLuint) \
  GLFN(void,            glVertexAttribPointer,     GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) \

#ifndef __EMSCRIPTEN__

#define GLFN(ret, name, ...) \
  typedef ret GLDECL name##proc(__VA_ARGS__); \
  extern name##proc *name;

GL_PROCS
#undef GLFN

#endif

/* This isn't needed anymore, clean up namespace */
#undef GLDECL

#endif