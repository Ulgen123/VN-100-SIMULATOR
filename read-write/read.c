#include <stdio.h>
#include <windows.h>
#include "read.h"
#include "write.h"
#include "parser.h"
#include "cksum.h"

int frequency = 1.0;
int new_frequency;

/**
 * @brief Reads data from a serial port and processes it.
 *
 * This function reads data from the specified serial port, processes the received data,
 * and writes it back to the serial port if it does not match specific conditions.
 *
 * ### Functionality:
 * - Reads data from the serial port using `ReadFile`.
 * - Processes the received data by passing it to a `parser` function.
 * - Writes the received data back to the serial port unless the message starts with "$VNRRG".
 * - Updates the system frequency if `new_frequency` differs from `frequency`.
 * - Handles and prints error messages if reading from the serial port fails.
 *
 * @param hSerial     The handle to the serial port.
 * @param readbuffer  The buffer to store the received data.
 * @param size        The size of the `readbuffer`.
 *
 * @return The number of bytes read from the serial port (DWORD).
 *
 * ### Error Handling:
 * - Prints an error message if the `ReadFile` function fails.
 *
 * ### Notes:
 * - The function assumes `readbuffer` is large enough to accommodate the received data.
 * - Ensures the data is null-terminated for safe string operations.
 *
 */

DWORD read(HANDLE hSerial, char *readbuffer, size_t size)
{
    DWORD bytesRead;
    if (ReadFile(hSerial, readbuffer, size - 1, &bytesRead, NULL))
    {
        if (bytesRead > 0)
        {
            readbuffer[bytesRead] = '\0'; // Null-terminator ekle
            printf("Received data: %s\n", readbuffer);
            parser(readbuffer);
            if (strncmp(readbuffer, "$VNRRG", 6) != 0)
            {
                write(hSerial, readbuffer);
            }
            if (new_frequency != frequency)
            {
                set_frequency(get_new_frequency());
            }
        }
    }
    else
    {
        printf("Error reading from COM port. Error code: %ld\n", GetLastError());
    }
    return bytesRead;
}