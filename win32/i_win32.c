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


// #ifndef UNICODE
// #define UNICODE
// #endif 

#include <windows.h>
#include <shellapi.h>

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "i_win32.h"
#include "i_sound.h"

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";

void* doomWindow = NULL;

static BOOL running;
static BITMAPINFO bitmapInfo;
static void* bitmapMemory;
static HBITMAP bitmapHandle;
static HDC bitmapDeviceContext;

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const char CLASS_NAME[]  = "win32doom";
    
    WNDCLASSEX wc = {0};
    wc.lpfnWndProc   = WindowCallback;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.cbSize = sizeof(WNDCLASSEX);

    
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Call to RegisterClassEx failed!", "Error!", 0);
        return 1;
    }
    // Create the window.
    
     doomWindow = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        "win32 DOOM",                  // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // , , width, height
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
    UpdateWindow(doomWindow);

    myargc = __argc; 
    myargv = __argv;  

    // D_DoomSetup();
    // D_DoomInit();

    // Run the message loop.
    running = true;
    MSG msg = {0};
    while (running)
    {
        if (GetMessage(&msg, NULL, 0, 0) < 0)
        {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
        // D_DoomStep();
    }

    return 0;
}

static void ResizeDIBSection(int width, int height)
{
    if(bitmapHandle)
    {
        DeleteObject(bitmapHandle);
    }
    if(!bitmapDeviceContext)
    {
        bitmapDeviceContext = CreateCompatibleDC(0);
    }


    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    HDC deviceContext = CreateCompatibleDC(0);
    bitmapHandle = CreateDIBSection(
        deviceContext,
        &bitmapInfo,
        DIB_RGB_COLORS,
        &bitmapMemory,
        0, 0);
}

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int result = 0;
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            running = false;
            break;
        }
        case WM_DESTROY: 
        {
            OutputDebugStringA("WM_DESTROY\n");
            running = false;
            break;
        }
        case WM_PAINT:
        {
            OutputDebugStringA("WM_PAINT\n");
            PAINTSTRUCT paintStruct;
            HDC deviceContext = BeginPaint(hwnd, &paintStruct);
            int x = paintStruct.rcPaint.left;
            int y = paintStruct.rcPaint.top;
            int height = paintStruct.rcPaint.bottom - paintStruct.rcPaint.top;
            int width = paintStruct.rcPaint.right - paintStruct.rcPaint.left;

            StretchDIBits(deviceContext,
                            x, y, width, height,
                            x, y, width, height,
                            bitmapMemory,
                            &bitmapInfo,
                            DIB_RGB_COLORS,
                            SRCCOPY);

            PatBlt(deviceContext, x, y, width, height, WHITENESS);
            EndPaint(hwnd, &paintStruct);
            break;
        }
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;
        }
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            ResizeDIBSection(width, height);
            break;
        }
        case WM_TIMER: // or settimer callback function
        {
            // if (wParam == timerId) 
            // { 
            //     return 0; 
            // } 
            OutputDebugStringA("WM_TIMER\n");
            break;
        }
        default:
        {
            result = DefWindowProcA(hwnd, uMsg, wParam, lParam);
        }

        return result;
    }
    
        
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

