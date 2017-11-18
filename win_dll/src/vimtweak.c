/*
 cl /LD vimtweak.c user32.lib
 gcc -shared -o vimtweak.dll vimtweak.c
 ------------------------------
 :call libcallnr("vimtweak.dll", "SetAlpha", 200)
 :call libcallnr("vimtweak.dll", "EnableMaximize", 1)
 :call libcallnr("vimtweak.dll", "EnableTopMost", 1)
*/

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#ifndef LWA_ALPHA
#define LWA_ALPHA 2
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifdef _MSC_VER
#define EXPORT _declspec(dllexport)
#else
#define EXPORT
#endif

static HWND hTopWindow = NULL;

BOOL CALLBACK
FindWindowProc(HWND hwnd, LPARAM lParam) {
  HWND* pphWnd = (HWND*)lParam;

  if (GetParent(hwnd)) {
    *pphWnd = NULL;
    return TRUE;
  }
  *pphWnd = hwnd;
  return FALSE;
}

static HWND
GetVimWindow() {
  if (hTopWindow) {
    return hTopWindow;
  }

  HMODULE hDllKernel32 = LoadLibraryA("kernel32");
  if (hDllKernel32) {
    HWND (WINAPI *pfnGetConsoleWindow)();
    pfnGetConsoleWindow
      = (HWND (WINAPI *)())
          GetProcAddress(hDllKernel32, "GetConsoleWindow");
    if (pfnGetConsoleWindow && (hTopWindow = (HWND) pfnGetConsoleWindow())) {
      ShowWindow(hTopWindow, SW_SHOW);
      return hTopWindow;
    }
  }
  DWORD dwThreadID = GetCurrentThreadId();
  EnumThreadWindows(dwThreadID, FindWindowProc, (LPARAM)&hTopWindow);
  return hTopWindow;
}

EXPORT LONG
SetAlpha(LONG nTrans) {
  HMODULE hDllUser32 = LoadLibraryA("user32");
  if (hDllUser32) {
    BOOL (WINAPI *pfnSetLayeredWindowAttributes)(HWND,DWORD,BYTE,DWORD);

    pfnSetLayeredWindowAttributes
      = (BOOL (WINAPI *)(HWND,DWORD,BYTE,DWORD))
          GetProcAddress(hDllUser32, "SetLayeredWindowAttributes");

    if (pfnSetLayeredWindowAttributes) {
      HWND hTop = GetVimWindow();
      if (hTop) {
        if (nTrans == 255) {
          SetWindowLong(hTop, GWL_EXSTYLE,
          GetWindowLong(hTop, GWL_EXSTYLE) & ~WS_EX_LAYERED); 
        } else {
          SetWindowLong(hTop, GWL_EXSTYLE,
          GetWindowLong(hTop, GWL_EXSTYLE) | WS_EX_LAYERED); 
          pfnSetLayeredWindowAttributes(hTop, 0, (BYTE)nTrans, LWA_ALPHA);
        }
      }
    }
    FreeLibrary(hDllUser32);
  }
  return GetLastError();
}

EXPORT LONG
EnableCaption(LONG bCaption) {
  HWND hTop;
  if (hTop = GetVimWindow()) {
    if (bCaption == 0)
      SetWindowLong(hTop, GWL_STYLE,
        GetWindowLong(hTop, GWL_STYLE) & ~WS_CAPTION); 
    else
      SetWindowLong(hTop, GWL_STYLE,
        GetWindowLong(hTop, GWL_STYLE) | WS_CAPTION); 
  }
  return GetLastError();
}

EXPORT LONG
EnableMaximize(LONG bEnable) {
  HWND hTop;
  if (hTop = GetVimWindow()) {
    if (bEnable == 0)
      SendMessage(hTop, WM_SYSCOMMAND, SC_RESTORE, 0);
    else
      SendMessage(hTop, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
  }
  return GetLastError();
}

EXPORT LONG
EnableTopMost(LONG bEnable) {
  HWND hTop;
  if (hTop = GetVimWindow()) {
    if (bEnable == 0)
      SetWindowPos(hTop, HWND_NOTOPMOST, 0, 0, 0, 0,
        SWP_NOSIZE | SWP_NOMOVE);
    else
      SetWindowPos(hTop, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOSIZE | SWP_NOMOVE);
  }
  return GetLastError();
}

static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam) {
	char ClassName[128];
	ClassName[GetClassNameA(hwnd, &ClassName[0], sizeof(ClassName) / sizeof(*ClassName))] = '\0';
	if (strcmp(ClassName, "Vim") == 0) {
		HWND* phVim = (HWND*)(lParam);
		*phVim = hwnd;
		return FALSE;
	}
	else {
		return TRUE;
	}
}

EXPORT int ToggleFullscreen(int arg) {
	HWND hWnd = NULL;
	if (EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, (LPARAM)(&hWnd)))
		hWnd = NULL;
	if (hWnd != NULL) {
		RECT* r;
		// 0: non full-screen, 1: full-screen
		switch ((int)(GetPropA(hWnd, "__full_state__")))
		{
		case 0:
			if (!(r = (RECT*)(GetPropA(hWnd, "__window_rect__")))) {
				r = (RECT*)HeapAlloc(GetProcessHeap(), 0, sizeof(RECT));

				// remove clientedge for vim textarea
				HWND hTextArea = FindWindowEx(hWnd, NULL, "VimTextArea", "Vim text area");
				DWORD dwExStyle = GetWindowLongPtr(hTextArea, GWL_EXSTYLE);
				SetWindowLongPtr(hTextArea, GWL_EXSTYLE, dwExStyle & ~WS_EX_CLIENTEDGE);
			}

			GetWindowRect(hWnd, r);
			SetPropA(hWnd, "__window_rect__", (HANDLE)(r));

			SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));

			SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);

			SetPropA(hWnd, "__full_state__", (HANDLE)(1));
			break;
		case 1:
			SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			SetClassLongPtr(hWnd, GCL_HBRBACKGROUND, (LONG)COLOR_BTNFACE);

			r = (RECT*)(GetPropA(hWnd, "__window_rect__"));
			SetWindowPos(hWnd, HWND_TOP, r->left, r->top, r->right - r->left, r->bottom - r->top, SWP_FRAMECHANGED);

			SetPropA(hWnd, "__full_state__", (HANDLE)(0));
			break;
		}
	}

	return 0;
}
// vi:set ts=8 sts=2 sw=2 tw=0 et:
