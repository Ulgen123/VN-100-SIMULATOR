#include <stdio.h>
#include <string.h>
#include <windows.h> //Gerekli kütüphaneleri ekledim
#include "serial_port.h"
/**
 * @brief Opens a serial port and sets up communication parameters.
 *
 * This function opens a specified serial port (`COM12` in this case) for both reading and writing operations.
 * It also sets custom timeout settings for read and write operations, ensuring the communication does not block indefinitely.
 * If the serial port cannot be opened or the timeout settings fail to be applied, an error message is printed.
 *
 * @note The function uses `CreateFile` to open the serial port and `SetCommTimeouts` to configure timeouts.
 *       The serial port being used in this example is `COM12` as part of a `com0com` virtual serial port pair (COM12 and COM13).
 *
 * @return void
 */
HANDLE hSerial;
void serial_port(void)
{
    LPCSTR portName = "\\\\.\\COM12"; // com0com ile com12 ve com13 açıldı. Com12’den com13’e mesaj atacağım.
    // Seri portu açtım
    hSerial = CreateFile(
        portName,
        GENERIC_READ | GENERIC_WRITE, // Okuma ve yazma erişimini ayarladım
        0,                            // Paylaşım yok
        NULL,                         // Varsayılan güvenlik özelliklerini ayarladım
        OPEN_EXISTING,                // Var olan bir portu açtım
        0,                            // Ek özellik yok
        NULL                          // Şablon yok
    );

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        printf("Error opening serial port. Error code: %ld\n", error);
        return;
    }
    printf("Serial port opened successfully.\n");

    // Zaman aşımı ayarları
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        printf("Failed to set timeouts. Error code: %ld\n", GetLastError());
    }
}