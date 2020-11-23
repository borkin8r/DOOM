#include <windows.h>

#include <io.h>

int access(const char* filename, int mode) {
    return _access(filename, mode);
}

int gettickcount() {
	return GetTickCount();
}

void sleep(int milliseconds) {
	Sleep(milliseconds);
}