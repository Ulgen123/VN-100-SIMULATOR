#ifndef READ_H
#define READ_H
#include <windows.h>
DWORD read(HANDLE hSerial, char *readbuffer, size_t size);
#endif /* READ_H */
