#include "input.h"
#include <windows.h>

struct CORE_input CORE_input;         /* Input for a tick */
static struct CORE_input CORE_inputv; /* Volatile core input */

/**
 * CORE_input_update
 *
 * Updates input with inputv
 */
#define KEY_COPY(k) if (CORE_inputv.keys.k) ++CORE_input.keys.k; else CORE_input.keys.k = 0;
void CORE_input_update(void)
{
  CORE_input.quit = CORE_inputv.quit;

  KEY_COPY(a);
  KEY_COPY(b);
  KEY_COPY(c);
  KEY_COPY(d);
  KEY_COPY(e);
  KEY_COPY(f);
  KEY_COPY(g);
  KEY_COPY(h);
  KEY_COPY(i);
  KEY_COPY(j);
  KEY_COPY(k);
  KEY_COPY(l);
  KEY_COPY(m);
  KEY_COPY(n);
  KEY_COPY(o);
  KEY_COPY(p);
  KEY_COPY(q);
  KEY_COPY(r);
  KEY_COPY(s);
  KEY_COPY(t);
  KEY_COPY(u);
  KEY_COPY(v);
  KEY_COPY(w);
  KEY_COPY(x);
  KEY_COPY(y);
  KEY_COPY(z);
  KEY_COPY(n0);
  KEY_COPY(n1);
  KEY_COPY(n2);
  KEY_COPY(n3);
  KEY_COPY(n4);
  KEY_COPY(n5);
  KEY_COPY(n6);
  KEY_COPY(n7);
  KEY_COPY(n8);
  KEY_COPY(n9);
  KEY_COPY(f1);
  KEY_COPY(f2);
  KEY_COPY(f3);
  KEY_COPY(f4);
  KEY_COPY(f5);
  KEY_COPY(f6);
  KEY_COPY(f7);
  KEY_COPY(f8);
  KEY_COPY(f9);
  KEY_COPY(f10);
  KEY_COPY(f11);
  KEY_COPY(f12);
  KEY_COPY(up);
  KEY_COPY(down);
  KEY_COPY(left);
  KEY_COPY(right);
  KEY_COPY(space);
  KEY_COPY(shift);
  KEY_COPY(enter);
  KEY_COPY(tab);
  KEY_COPY(escape);
  KEY_COPY(backspace);

  CORE_input.mouse.x = CORE_inputv.mouse.x;
  CORE_input.mouse.y = CORE_inputv.mouse.y;
  if (CORE_inputv.mouse.l) ++CORE_input.mouse.l; else CORE_input.mouse.l = 0;
  if (CORE_inputv.mouse.r) ++CORE_input.mouse.r; else CORE_input.mouse.r = 0;

}

#ifdef _WIN32
  #include "input_win32.c"
#else
  #error Platform not supported (CORE::input)
#endif