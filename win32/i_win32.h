#ifndef __I_WIN32__
#define __I_WIN32__
	extern void* doomWindow;

	void Win32RenderScreen(unsigned char* screen);
	void UploadNewPalette(unsigned char* palette);
	void Win32FillSoundBuffer(short* mixBuffer, int bufferSize);
	void InitializeSound(int samplesPerSecond, int bufferSize);
	void Win32PlaySoundBuffer();
	void ReadMessages();
#endif