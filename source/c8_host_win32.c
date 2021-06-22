#if defined(NO_SDL) && defined(WIN32)

#include "c8_host.h"

#include <Windows.h>

#include <stdint.h>
#include <stdio.h>

struct
{
    int initialized;
    const wchar_t* title;
    int height, width, scale;
    BITMAPINFO* dib_info;
    HANDLE consoleHandle;
    HWND hwnd;
    HDC hdc;
    CONSOLE_CURSOR_INFO cursor_info;
} host = { 0 };

inline BITMAPINFO* c8_host_create_dib_info(void);

// **************************
// **   PUBLIC FUNCTIONS   **
// **************************

int c8_host_init(const wchar_t* title, int width, int height, int scale)
{
    if (host.initialized) return TRUE;

    host.title = title;
    host.width = width;
    host.height = height;
    host.scale = scale;

    host.consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (host.consoleHandle == NULL) return FALSE;
    GetConsoleCursorInfo(host.consoleHandle, &host.cursor_info);
    host.cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(host.consoleHandle, &host.cursor_info);

    SetConsoleTitleW(title);

    Sleep(100);
    host.hwnd = FindWindowW(NULL, title);
    if (host.hwnd == NULL) return FALSE;
    host.hdc = GetDC(host.hwnd);
    if (host.hdc == NULL) return FALSE;

    host.dib_info = c8_host_create_dib_info();
    if (host.dib_info == NULL) return FALSE;

    host.initialized = TRUE;
    return TRUE;
}

void c8_host_cleanup(void)
{
    if (!host.initialized) return;

    ReleaseDC(host.hwnd, host.hdc);
    DeleteDC(host.hdc);
    free(host.dib_info);

    host.cursor_info.bVisible = TRUE;
    SetConsoleCursorInfo(host.consoleHandle, &host.cursor_info);

    host.initialized = FALSE;
}

uint64_t c8_host_get_100nanoseconds(void)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return (((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime);
}

c8_word c8_host_get_keys(void)
{
    c8_word keys = 0;
    keys |= !!(GetKeyState('1') & ~1) * C8_KEY_1;
    keys |= !!(GetKeyState('2') & ~1) * C8_KEY_2;
    keys |= !!(GetKeyState('3') & ~1) * C8_KEY_3;
    keys |= !!(GetKeyState('4') & ~1) * C8_KEY_C;
    keys |= !!(GetKeyState('Q') & ~1) * C8_KEY_4;
    keys |= !!(GetKeyState('W') & ~1) * C8_KEY_5;
    keys |= !!(GetKeyState('E') & ~1) * C8_KEY_6;
    keys |= !!(GetKeyState('R') & ~1) * C8_KEY_D;
    keys |= !!(GetKeyState('A') & ~1) * C8_KEY_7;
    keys |= !!(GetKeyState('S') & ~1) * C8_KEY_8;
    keys |= !!(GetKeyState('D') & ~1) * C8_KEY_9;
    keys |= !!(GetKeyState('F') & ~1) * C8_KEY_E;
    keys |= !!(GetKeyState('Z') & ~1) * C8_KEY_A;
    keys |= !!(GetKeyState('X') & ~1) * C8_KEY_0;
    keys |= !!(GetKeyState('C') & ~1) * C8_KEY_B;
    keys |= !!(GetKeyState('V') & ~1) * C8_KEY_F;
    return keys;
}

void c8_host_make_sound(void)
{
    Beep(1000, 1);
}

int c8_host_is_not_quit(void)
{
    return !(GetKeyState(VK_ESCAPE) & ~1);
}

int c8_host_is_speed_up(void)
{
    return !!(GetKeyState(VK_OEM_PLUS) & ~1);
}

int c8_host_is_speed_down(void)
{
    return !!(GetKeyState(VK_OEM_MINUS) & ~1);
}

int c8_host_is_speed_reset(void)
{
    return !!(GetKeyState('0') & ~1);
}

int c8_host_is_paused(void)
{
    return !!(GetKeyState(VK_SPACE) & ~1);
}

int c8_host_is_program_reset(void)
{
    return !!(GetKeyState(VK_BACK) & ~1);
}

void c8_host_render(c8_byte* buf)
{
    StretchDIBits(host.hdc, 0, 0, host.width * host.scale, host.height * host.scale, 0, 0, 64, 32, buf, host.dib_info, DIB_RGB_COLORS, SRCCOPY);
}

void c8_host_sleep(unsigned long milliseconds)
{
    Sleep(milliseconds);
}

// ***************************
// **   PRIVATE FUNCTIONS   **
// ***************************

inline BITMAPINFO* c8_host_create_dib_info(void)
{
    BITMAPINFO* info = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
    if (info == NULL) return NULL;

    info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth = 64;
    info->bmiHeader.biHeight = -32;
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = 1;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biSizeImage = 64 / 8 * 32;
    info->bmiHeader.biXPelsPerMeter = 200;
    info->bmiHeader.biYPelsPerMeter = 200;
    info->bmiHeader.biClrUsed = 2;
    info->bmiHeader.biClrImportant = 2;
    info->bmiColors[0].rgbRed = 128;
    info->bmiColors[0].rgbBlue = 128;
    info->bmiColors[0].rgbGreen = 128;
    info->bmiColors[0].rgbReserved = 0;
    info->bmiColors[1].rgbRed = 255;
    info->bmiColors[1].rgbBlue = 255;
    info->bmiColors[1].rgbGreen = 255;
    info->bmiColors[1].rgbReserved = 0;

    return info;
}

#endif