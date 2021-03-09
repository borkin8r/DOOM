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
#include <memory.h>

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "i_win32.h"
#include "i_sound.h"
#include "v_video.h"

static const char
rcsid[] = "$Id: i_win32.c,v 0.1 2020/11/30 22:45:10 b1 Exp $";

void* doomWindow = NULL;

static BOOL running;
static BITMAPINFO* bitmapInfo;
void* bitmapMemory;
static int bitmapWidth;
static int bitmapHeight;

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void ResizeDIBSection(int width, int height);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR pCmdLine, int nCmdShow)
{
    ResizeDIBSection(SCREENWIDTH, SCREENHEIGHT);

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
        CW_USEDEFAULT, CW_USEDEFAULT, SCREENWIDTH * 2, SCREENHEIGHT * 2, // , , width, height TODO: test with multiply global
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (doomWindow == NULL)
    {
        return 0;
    }

    bitmapInfo = VirtualAlloc(0, sizeof(BITMAPINFOHEADER) * sizeof(RGBQUAD) * 256, MEM_COMMIT, PAGE_READWRITE);
    if (bitmapInfo == NULL)
    {
        return 0;
    }

    bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo->bmiHeader.biWidth = SCREENWIDTH;
    bitmapInfo->bmiHeader.biHeight = -SCREENHEIGHT;
    bitmapInfo->bmiHeader.biPlanes = 1;
    bitmapInfo->bmiHeader.biBitCount = 8;
    bitmapInfo->bmiHeader.biCompression = BI_RGB;

    ShowWindow(doomWindow, nCmdShow);
    UpdateWindow(doomWindow);

    myargc = __argc; 
    myargv = __argv;  

    D_DoomSetup();
    D_DoomInit();

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
            D_DoomStep();
        } 
    }

    return 0;
}

static void ResizeDIBSection(int width, int height)
{
    if(bitmapMemory)
    {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    int bytesPerPixel = 1;

// bitmapInfo needs to be allocated before calling
    // bitmapInfo->bmiHeader.biWidth = SCREENWIDTH;
    // bitmapInfo->bmiHeader.biHeight = -SCREENHEIGHT;

    bitmapWidth = width;
    bitmapHeight = height;
    
    int bitmapMemorySize = (bitmapWidth * bitmapHeight) * bytesPerPixel;
    printf("bitmapMemorySize: %d", bitmapMemorySize);
    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
}

//wparam = int64 , lparam = unsigned int64
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

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int windowWidth = clientRect.right - clientRect.left;
            int windowHeight = clientRect.bottom - clientRect.top;
            StretchDIBits(deviceContext,
                            x, y, windowWidth, windowHeight,
                            x, y, windowWidth, windowHeight,
                            bitmapMemory, // screens[0] memcpy destination
                            bitmapInfo,
                            DIB_RGB_COLORS,
                            SRCCOPY);

            PatBlt(deviceContext, x, y, windowHeight, windowHeight, WHITENESS);
            EndPaint(hwnd, &paintStruct);
            break;
        }
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            break;
        }
        case WM_KEYDOWN:
        {
            // toAscii()?
            event_t event = {
                ev_keydown,
                wParam,
                0,
                0
            };
            D_PostEvent(&event);
            break;
        }
        // case WM_SIZE:
        // {
        //     OutputDebugStringA("WM_SIZE\n");
        //     RECT clientRect;
        //     GetClientRect(hwnd, &clientRect);
        //     int width = clientRect.right - clientRect.left;
        //     int height = clientRect.bottom - clientRect.top;
        //     ResizeDIBSection(width, height);
        //     break;
        // }
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

// called from I_FinishUpdate in i_video.c
void Win32RenderScreen(unsigned char* screen) //window width? window height?
{
    RECT clientRect;
    GetClientRect(doomWindow, &clientRect);
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;
    // TODO: write to bitmapMemory; screens[x] -> bitmapMemory
    // probably need to check endianess
    errno_t err;
    // 1 is bytes per pixel
    int destinationSize = 1 * SCREENWIDTH * SCREENHEIGHT;
    int srcCount = SCREENWIDTH * SCREENHEIGHT;
    err = memcpy_s(bitmapMemory, destinationSize, screen, srcCount);
    if (err)
    {
        printf("Error executing memcpy_s.\n");
    }
    HDC deviceContext = GetDC(doomWindow);
    StretchDIBits(deviceContext,
                            0, 0, windowWidth, windowHeight, // destination
                            0, 0, windowWidth, windowHeight, // source
                            bitmapMemory, // screens[0] memcpy destination; should be same size/format
                            bitmapInfo,
                            DIB_RGB_COLORS,
                            SRCCOPY);
    ReleaseDC(doomWindow, deviceContext);
}

void UploadNewPalette(unsigned char* palette) {
    //TODO: bitmapInfo->bmiColors
    int c;
    for (int i=0 ; i<256 ; i++) // sizeof(int) == sizeof(RGBQUAD) ?
    {
        c = gammatable[usegamma][*palette++];
        bitmapInfo->bmiColors[i].rgbRed = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        bitmapInfo->bmiColors[i].rgbGreen = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        bitmapInfo->bmiColors[i].rgbBlue = (c<<8) + c;
    }
}