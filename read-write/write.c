#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "write.h"
#include "serial_port.h"

/**
 * @brief Sends a message to a serial port.
 *
 * This function transmits a null-terminated string (`fnlmessage`) to the specified serial port (`Serial`).
 * It uses the `WriteFile` function to perform the transmission and returns the number of bytes successfully written.
 *
 * If the write operation fails, an error message is printed to indicate the failure.
 *
 * @param Serial A handle to the serial port, typically obtained from `CreateFile`.
 * @param fnlmessage A pointer to the null-terminated string to be sent over the serial port.
 *                   The length of the string is calculated using `strlen`.
 *
 * @return DWORD The number of bytes successfully written to the serial port.
 *               If the write operation fails, this value may be 0.
 *
 * @note Ensure that the serial port is properly configured before calling this function.
 *       The function does not handle retries or additional error recovery mechanisms.
 */

DWORD write(HANDLE Serial, const char *fnlmessage)
{
    DWORD bytesWritten;
    if (!WriteFile(Serial, fnlmessage, strlen(fnlmessage), &bytesWritten, NULL))
    {
        printf("Error writing to serial port.\n");

    }
    return bytesWritten;
}