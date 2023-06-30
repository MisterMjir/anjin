#include "input.h"
#include "keys_win32.h"
#include <windows.h>
#include <windowsx.h>

/**
 * CORE_input_loop
 *
 * Waits for a thread to finish running
 */
void CORE_input_loop(void)
{
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    DispatchMessage(&msg);

    switch (msg.message) {
      case WM_KEYDOWN:
        switch (msg.wParam) {
          #define X(key, sym) case sym: CORE_inputv.keys.key = 1; break;
          KEYS
          #undef X
        }
        break;
      case WM_KEYUP:
        switch (msg.wParam) {
          #define X(key, sym) case sym: CORE_inputv.keys.key = 0; break;
          KEYS
          #undef X
        }
        break;
      case WM_MOUSEMOVE:
        CORE_inputv.mouse.x = GET_X_LPARAM(msg.lParam);
        CORE_inputv.mouse.y = GET_Y_LPARAM(msg.lParam);
        break;
      case WM_LBUTTONDOWN: CORE_inputv.mouse.l = 1; break;
      case WM_LBUTTONUP:   CORE_inputv.mouse.l = 0; break;
      case WM_RBUTTONDOWN: CORE_inputv.mouse.r = 1; break;
      case WM_RBUTTONUP:   CORE_inputv.mouse.r = 0; break;
    }
  }
  /* The loop exits only when the message is WM_QUIT */
  CORE_inputv.quit = 1;
}