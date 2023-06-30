#include "gll.h"
#include "log/log.h"

#define GLFN(ret, name, ...) \
  name##proc *name;

GL_PROCS
#undef GLFN

static void * get_gl_proc_addr(const char* name)
{
  void * p = (void *) wglGetProcAddress(name);
  if (p == 0 || p == (void *) 0x1 || p == (void *) 0x2 || p == (void *) 0x3 || p == (void *) -1) {
    HMODULE module = LoadLibraryA("opengl32.dll");
    if (!module) {
      LOG_msg("Cannot open opengl32.dll");
      return NULL;
    }
    p = (void *) GetProcAddress(module, name);
  }

  return p;
}

int GL_gll(void)
{
  #define GLFN(ret, name, ...) \
    name = (name##proc *) get_gl_proc_addr(#name); \
    if (!name) {LOG_msg("Failed to load " #name "\n"); return -1;}

  GL_PROCS
  #undef GLFN

  return 0;
}