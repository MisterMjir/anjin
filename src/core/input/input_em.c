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

void CORE_input_loop(void)
{
  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keydown_callback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, keyup_callback);
}