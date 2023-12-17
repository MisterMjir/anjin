#include "input.h"
#include <emscripten/html5.h>
#include "keys_em.h"

EM_BOOL keydown_callback(int event_type, const EmscriptenKeyboardEvent *key_event, void *user_data)
{
  switch (key_event->keyCode) {
    #define X(key, sym) case sym: CORE_inputv.keys.key = 1; break;
    KEYS
    #undef X
  }

  return EM_TRUE;
}

EM_BOOL keyup_callback(int event_type, const EmscriptenKeyboardEvent *key_event, void *user_data)
{
  switch (key_event->keyCode) {
    #define X(key, sym) case sym: CORE_inputv.keys.key = 0; break;
    KEYS
    #undef X
  }

  return EM_TRUE;
}

EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
  CORE_inputv.mouse.x = mouseEvent->targetX;
  CORE_inputv.mouse.y = mouseEvent->targetY;

  return EM_TRUE;
}

EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
  switch (mouseEvent->button) {
    case 0:
      CORE_inputv.mouse.l = 1;
      break;
    case 2:
      CORE_inputv.mouse.r = 1;
      break;
  }

  return EM_TRUE;
}

EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
  switch (mouseEvent->button) {
    case 0:
      CORE_inputv.mouse.l = 0;
      break;
    case 2:
      CORE_inputv.mouse.r = 0;
      break;
  }

  return EM_TRUE;
}

void CORE_input_loop(void)
{
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keydown_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keyup_callback);

  emscripten_set_mousemove_callback("#ANJIN_WINDOW", NULL, EM_TRUE, mousemove_callback);
  emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mousedown_callback);
  emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, mouseup_callback);
}