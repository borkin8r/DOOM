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
rcsid[] = "$Id: i_win32.c,v 0.1 2020/11/30 22:45:10 b1 Exp $";

void* doomWindow = NULL;

static BOOL running;
static BITMAPINFO bitmapInfo;
static void* bitmapMemory;
static int bitmapWidth;
static int bitmapHeight;

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR pCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WindowCallback;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "win32doom";
    wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;

    
    if (!RegisterClassA(&wc))
    {
        MessageBox(NULL, "Call to RegisterClass failed!", "Error!", 0);
        return 1;
    }
    // Create the window.
    
     doomWindow = CreateWindowExA(
        0,                              // Optional window styles.
        wc.lpszClassName,                     // Window class
        "win32 DOOM",                  // Window text
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,            // Window style
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
    MSG message = {0};
    while (running)
    {
        while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
        {
            if(message.message == WM_QUIT)
            {
                running = false;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
            // D_DoomStep();
        } 
        // write to bitmap memory
        // updatewindow() to blit to screen
    }

    return 0;
}

static void ResizeDIBSection(int width, int height)
{
    if(bitmapMemory)
    {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    bitmapWidth = width;
    bitmapHeight = height;

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    int bytesPerPixel = 4;
    int bitmapMemorySize = (bitmapWidth * bitmapHeight) * bytesPerPixel;
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
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
            // update window begin?

            PAINTSTRUCT paintStruct;
            HDC deviceContext = BeginPaint(hwnd, &paintStruct);
            int x = paintStruct.rcPaint.left;
            int y = paintStruct.rcPaint.top;

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int windowWidth = clientRect.right - clientRect.left;
            int windowHeight = clientRect.bottom - clientRect.top;
            StretchDIBits(deviceContext,
                            x, y, windowWidth, windowHeight,
                            x, y, windowWidth, windowHeight,
                            bitmapMemory,
                            &bitmapInfo,
                            DIB_RGB_COLORS,
                            SRCCOPY);
            //update window end

            PatBlt(deviceContext, x, y, windowHeight, windowHeight, WHITENESS);
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

