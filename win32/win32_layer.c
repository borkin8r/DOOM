#include <io.h>

int access(const char* filename, int mode) {
    return _access(filename, mode);
}