#include <windows.h>


#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int access(const char* filename, int mode) {
    return _access(filename, mode);
}

int gettickcount() {
	return GetTickCount();
}