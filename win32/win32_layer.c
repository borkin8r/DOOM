#include <io.h>
#include <windows.h>

int access(const char* filename, int mode) {
    return _access(filename, mode);
}

int gettickcount() {
	return GetTickCount();
}