#include "write.h"
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "serial_port.h"
/**
 * @brief Sends a message through a specified serial port.
 *
 * This function writes a message to the serial port represented by the provided handle (`hSerial`). 
 * The message to be sent is passed as a parameter (`message`) and written using the Windows API `WriteFile` function.
 * If the write operation fails, an error message is printed to the console.
 *
 * @param hSerial A handle to the serial port to which the message will be written.
 * @param message A pointer to the message string to be sent through the serial port.
 */
extern HANDLE hSerial;
char finalmessage[256];
void write(HANDLE hSerial, const char *message )
{
    DWORD bytesWritten;
if (!WriteFile(hSerial, finalmessage, strlen(finalmessage), &bytesWritten, NULL))
            {
                printf("Error writing to serial port.\n");
            }
}