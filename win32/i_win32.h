#ifndef __I_WIN32__
#define __I_WIN32__
	extern void* doomWindow;

	void Win32RenderScreen(unsigned char* screen);
	void UploadNewPalette(unsigned char* palette);
#endif