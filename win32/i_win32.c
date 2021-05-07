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
#include <windowsx.h>
#include <shellapi.h>
#include <memory.h>
#include <dsound.h>

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "i_win32.h"
#include "i_sound.h"
#include "i_video.h"
#include "v_video.h"

static const char
rcsid[] = "$Id: i_win32.c,v 0.1 2020/11/30 22:45:10 b1 Exp $";

void* doomWindow = NULL;

static BOOL running;
static BITMAPINFO* bitmapInfo;
static void* bitmapMemory;
static int bitmapWidth;
static int bitmapHeight;
static LPDIRECTSOUNDBUFFER SecondaryBuffer;
static int SecondaryBufferSize = 0;
static int SamplesPerSecond = 0;
static __int32 RunningSampleIndex;
static int BytesPerSample = sizeof(__int16)*2;

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

    InitializeSound(48000, 48000*sizeof(short)*2);

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

    // infinite loop until program exits
    running = true;
    D_DoomLoop();

    return 0;
}

void ReadMessages()
{
    if (!running)
    {
        return;
    }

    MSG message = {0};
    while(PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        if (message.message == WM_QUIT)
        {
            running = false;
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
    } 
}

static void ResizeDIBSection(int width, int height)
{
    if (bitmapMemory)
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
        case WM_KEYUP:
        {
            // toAscii()?
            event_t event = {
                ev_keyup,
                wParam,
                0,
                0
            };
            D_PostEvent(&event);
            break;
        }
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        {
            // TODO: send mouseX, mouseY too?
            I_PostButtonEvent(wParam, true); // button code, isPress params
            break;
        }
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        {
            // TODO: send mouseX, mouseY too?
            I_PostButtonEvent(wParam, false); // button code,  isPress params
            break;
        }
        case WM_MOUSEMOVE:
        {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            I_PostMotionEvent(wParam, mouseX, mouseY); // postevent and set last x/y and mousemoved
            // event_t event = {
            //     ev_mouse,
            //     wParam,
            //     mouseX,
            //     mouseY
            // };
            // D_PostEvent(&event);
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

void Win32PlaySoundBuffer()
{
    IDirectSoundBuffer_Play(SecondaryBuffer, 0, 0, DSBPLAY_LOOPING);
}

void Win32FillSoundBuffer(short* mixBuffer, int bufferSize)
{
    DWORD PlayCursor;
    DWORD WriteCursor;
    if (SUCCEEDED(IDirectSoundBuffer_GetCurrentPosition(SecondaryBuffer, &PlayCursor, &WriteCursor)))
    {
        DWORD ByteToLock = ((RunningSampleIndex*BytesPerSample) % SecondaryBufferSize);
        DWORD TargetCursor = ((PlayCursor + ((SamplesPerSecond / 15)*BytesPerSample)) %
                                SecondaryBufferSize);

        DWORD BytesToWrite;
        if(ByteToLock > TargetCursor)
        {
            BytesToWrite = (SecondaryBufferSize - ByteToLock);
            BytesToWrite += TargetCursor;
        }
        else
        {
            BytesToWrite = TargetCursor - ByteToLock;
        }


        void* Region1;
        DWORD Region1Size;
        void* Region2;
        DWORD Region2Size;

        if (SUCCEEDED(IDirectSoundBuffer_Lock(
            SecondaryBuffer,
            ByteToLock, BytesToWrite,
            &Region1, &Region1Size,
            &Region2, &Region2Size,
            0)))
        {
            DWORD Region1SampleCount = Region1Size/BytesPerSample;
             __int16 *SampleOut = (__int16*)Region1;
            for (DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
            {
                *SampleOut++ = *mixBuffer++; //left stereo audio frequency
                *SampleOut++ = *mixBuffer++; //right stereo audio frequency

                ++RunningSampleIndex;
            }

            DWORD Region2SampleCount = Region2Size/BytesPerSample;
            SampleOut = (__int16*)Region2;
            for (DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
            {
                *SampleOut++ = *mixBuffer++; //left stereo audio frequency
                *SampleOut++ = *mixBuffer++; //right stereo audio frequency

                ++RunningSampleIndex;
            }
            
            IDirectSoundBuffer_Unlock(SecondaryBuffer, Region1, Region1Size, Region2, Region2Size);
        }
    }
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

typedef HRESULT WINAPI direct_sound_create(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);

void InitializeSound(int samplesPerSecond, int bufferSize)
{
    SamplesPerSecond = samplesPerSecond;
    SecondaryBufferSize = bufferSize;

    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

    if(DSoundLibrary)
    {
        direct_sound_create* DirectSoundCreate = (direct_sound_create *) GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        LPDIRECTSOUND DirectSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0,  &DirectSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {0};
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = 2;
            WaveFormat.nSamplesPerSec = samplesPerSecond; //48k? or 11k?
            WaveFormat.wBitsPerSample = 16;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
            WaveFormat.cbSize = 0;

            if (SUCCEEDED(IDirectSound_SetCooperativeLevel(DirectSound, doomWindow, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC BufferDescription = {0};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if (SUCCEEDED(IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription, &PrimaryBuffer, 0)))
                {

                    
                    if(SUCCEEDED(IDirectSoundBuffer_SetFormat(PrimaryBuffer, &WaveFormat)))
                    {
                        OutputDebugStringA("Primary buffer format was set.\n");
                    } 
                }
            }
            
            DSBUFFERDESC BufferDescription = {0};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = bufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;
            HRESULT Error = IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription, &SecondaryBuffer, 0);
            if (SUCCEEDED(Error))
            {
                OutputDebugStringA("Secondary buffer created successfully.\n");
            }
            

        }
    }
}