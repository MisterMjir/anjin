#include <windows.h>
#include <stdio.h>

struct info {
  int  fs;
  LONG style;
  int  maximized;
  RECT r;
};

struct CORE_window {
  HWND        h;
  HDC         dc;
  HGLRC       rc;
  struct info info;
};

static HINSTANCE main_instance;

/**
 * window_proc
 *
 * Window callback
 *
 * @param hwnd
 * @param umsg
 * @param wparam
 * @param lparam
 * @return
 */
LRESULT CALLBACK window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
  RECT r;

  switch (umsg) {
    case WM_SIZE:
      if (CORE_window_cb_size) {
        GetClientRect(hwnd, &r);
        CORE_window_cb_size(r.right - r.left, r.bottom - r.top);
      }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(hwnd, umsg, wparam, lparam);
}

/**
 * default_proc
 *
 * Window callback for a temporary window
 *
 * @param hwnd
 * @param umsg
 * @param wparam
 * @param lparam
 * @return
 */
LRESULT CALLBACK default_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
  return DefWindowProc(hwnd, umsg, wparam, lparam);
}

/**
 * register_core
 *
 * Registers the core window class
 *
 * @param wc Reference to a window class
 * @return   Success
 */
static int register_core(WNDCLASS *wc)
{
  wc->style = 0;
  wc->lpfnWndProc = window_proc;
  wc->cbClsExtra = 0;
  wc->cbWndExtra = 0;
  wc->hInstance = main_instance;
  wc->hIcon = NULL;
  wc->hCursor = LoadCursor(NULL, IDC_ARROW);
  wc->hbrBackground = NULL;
  wc->lpszMenuName = NULL;
  wc->lpszClassName = "window_core";

  if (!RegisterClass(wc)) {
    fprintf(stderr, "Could not register class window_core. Error %d\n", GetLastError());
    return -1;
  }

  return 0;
}

/**
 * register_temp
 *
 * Registers the temporary window class
 *
 * @param wc Reference to a window class
 * @return   Success
 */
static int register_temp(WNDCLASS *wc)
{
  wc->style = CS_HREDRAW | CS_VREDRAW;
  wc->lpfnWndProc = default_proc;
  wc->cbClsExtra = 0;
  wc->cbWndExtra = 0;
  wc->hInstance = main_instance;
  wc->hIcon = NULL;
  wc->hCursor = LoadCursor(NULL, IDC_ARROW);
  wc->hbrBackground = NULL;
  wc->lpszMenuName = NULL;
  wc->lpszClassName = "window_temp";

  if (!RegisterClass(wc)) {
    fprintf(stderr, "Could not register class window_temp. Error %d\n", GetLastError());
    return -1;
  }

  return 0;
}

/**
 * CORE_window_init
 *
 * Initializes everything needed for window creation
 *
 * @return Success
 */
int CORE_window_init(void)
{
  int res;
  WNDCLASS wc;

  if (!(main_instance = GetModuleHandle(NULL))) {
    fprintf(stderr, "Could not get the module. Error %d\n", GetLastError());
    return -1;
  }

  res = 0;
  if ((res = register_core(&wc))) return res;
  if ((res = register_temp(&wc))) return res;

  return res;
}

/**
 * CORE_window_create
 *
 * Creates a window
 *
 * @return A pointer to the newly created window
 */
#define WIN_STYLE_FIXED (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX)
#define WIN_STYLE_FREE  (WS_OVERLAPPEDWINDOW)
#define WIN_STYLE WIN_STYLE_FREE
struct CORE_window * CORE_window_create(int w, int h)
{
  struct CORE_window *win;

  if (!(win = malloc(sizeof(*win)))) {
    fprintf(stderr, "Out of memory\n");
    return NULL;
  }

  RECT wr = {0, 0, w, h};
  AdjustWindowRect(&wr, WIN_STYLE, FALSE);

  win->h = CreateWindow(
    "window_core", "Window title", WIN_STYLE,
    CW_USEDEFAULT, CW_USEDEFAULT,
    wr.right - wr.left, wr.bottom - wr.top,
    NULL, NULL, main_instance, NULL
  );

  win->dc = GetDC(win->h);

  win->info.fs = 0;

  ShowWindow(win->h, SW_NORMAL);

  return win;
}

static void destroy_icon(struct CORE_window *w)
{
  HICON i;

  i = (HICON) GetClassLongPtr(w->h, GCLP_HICON);
  if (i) {
    DestroyIcon(i);
    i = NULL;
  }
}

void CORE_window_destroy(struct CORE_window *w)
{
  destroy_icon(w);
  ReleaseDC(w->h, w->dc);
  DestroyWindow(w->h);
  free(w);
}

void CORE_window_swap_buffers(struct CORE_window *w)
{
  SwapBuffers(w->dc);
}

void CORE_window_fullscreen(struct CORE_window *w, int t)
{
  MONITORINFO mi;

  if (t == w->info.fs) return;

  if (!t) { /* Exit fullscreen */
    w->info.fs = 0;
    SetWindowLong(w->h, GWL_STYLE, w->info.style);

    SetWindowPos(
      w->h, NULL, w->info.r.left, w->info.r.top,
      w->info.r.right - w->info.r.left, w->info.r.bottom - w->info.r.top,
      SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
    );

    if (w->info.maximized)
      SendMessage(w->h, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
  }
  else { /* Enter fullscreen */
    w->info.fs = 1;
    /* Save window information */
    if ((w->info.maximized = IsZoomed(w->h))) {
      SendMessage(w->h, WM_SYSCOMMAND, SC_RESTORE, 0);
    }
    w->info.style = GetWindowLong(w->h, GWL_STYLE);
    GetWindowRect(w->h, &w->info.r);
    /* Set fullscreen stuff */
    SetWindowLong(
      w->h, GWL_STYLE,
      w->info.style & ~(WS_CAPTION | WS_THICKFRAME)
    );

    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(w->h, MONITOR_DEFAULTTONEAREST), &mi);
    SetWindowPos(
      w->h, NULL, mi.rcMonitor.left, mi.rcMonitor.top,
      mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
      SWP_NOZORDER | SWP_NOACTIVATE |SWP_FRAMECHANGED
    );
  }
}

void CORE_window_icon(struct CORE_window *w, const char *fpath)
{
  destroy_icon(w);
  SetClassLongPtr(
    w->h,
    GCLP_HICON,
    (LONG_PTR) ((HICON) LoadImage(
      NULL,
      fpath,
      IMAGE_ICON,
      0,
      0,
      LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED
    ))
  );
}

void CORE_window_title(struct CORE_window *w, const char *s)
{
  SetWindowText(w->h, s);
}