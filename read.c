#include "read.h"
#include "write.h"
#include <stdio.h>
#include <windows.h>
#include "parser.h"
//BRIEF EKLE

extern HANDLE hSerial;
extern char readBuffer[256];
void read(HANDLE hSerial, const char *buffer)
{
DWORD bytesRead;
        if (ReadFile(hSerial, readBuffer, sizeof(readBuffer) - 1, &bytesRead, NULL))
        {
            if (bytesRead > 0)
            {
                readBuffer[bytesRead] = '\0'; // Null-terminator ekle
                printf("Received data: %s\n", readBuffer);
                // 1. Check if message has "$" and "*"
                // 1.1 Check if "$" is the first letter
                // 2. $msg*CRC
                // 2.1 extract the msg
                // 2.2 extract the CRC
                // 3. Check if CRC is valid (of msg) error:

                // 4. Parse the message
                // 4.1 Parse the header "VNWRG" (extract with ",")
                //      4.2 Parse the message id = 07
                //          4.3 Parse the payload = variable (variable = period)
                parser(readBuffer);
                write(hSerial, readBuffer);
                CloseHandle(hSerial);
            }
        }
        else
        {
            printf("Error reading from COM port. Error code: %ld\n", GetLastError());
        }


}