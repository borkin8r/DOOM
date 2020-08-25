#ifndef WIN32LAYER
#define WIN32LAYER

#define R_OK 0x04
#define X_OK 0x00 //checks for existence same as execute access?

int access(const char* filename, int mode);

int gettickcount();

#endif