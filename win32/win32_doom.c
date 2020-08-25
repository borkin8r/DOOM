// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	windows platform entry point for starting doom
//	 
//    
//-----------------------------------------------------------------------------


#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "win32_doom.h"
#include "i_sound.h"

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";

void* doomWindow = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevHInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"win32doom";
    
    WNDCLASSEXW wc = {0};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.cbSize = sizeof(WNDCLASSEXW);

    
    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL,
            L"Call to RegisterClassExW failed!",
            L"Windows Desktop Guided Tour",
            0);

        return 1;
    }
    // Create the window.
    
     doomWindow = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"win32 DooM",                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (doomWindow == NULL)
    {
        return 0;
    }

    ShowWindow(doomWindow, nCmdShow);

    D_DoomInit();

    // Run the message loop.
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        D_DoomStep();
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY: 
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT:
        {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);



                FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

                EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER: 
        {
            if (wParam == timerId) 
            { 
                return 0; 
            } 
        }
    }
    
        
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

