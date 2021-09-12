/* Name: gdiTemplate.h v0.0.0
 * Date: 2021-09-11
 * Intro: A header to use gdi with c easier.
 */
#ifndef _GDITEMPLATE_H
#define _GDITEMPLATE_H 0x000000

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

DWORD WINAPI threadProc(LPVOID lpParamter);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif

#endif
