#include <stdio.h>
#include <string.h>
#include <windows.h> //Gerekli kütüphaneleri ekledim
#include <time.h>
#include "write.h"
#include "read.h"
#include "parser.h"
#include "cksum.h"
#include "serial_port.h"

extern HANDLE hSerial;
char readBuffer[256];
typedef struct
{
    float yaw;
    float pitch;
    float roll;
    float MagX;
    float MagY;
    float MagZ;
    float AccelX;
    float AccelY;
    float AccelZ;
    float GyroX;
    float GyroY;
    float GyroZ;
} YMR_package_t;

char *packageStartDelimiter = "$";
char *packageHeader = "VNYMR";
char *packageTail = "*";

int main()
{
    serial_port();
    YMR_package_t msg;
    msg.yaw = 1.0f;
    msg.pitch = 2.0f;
    msg.roll = 3.0f;
    msg.MagX = 4.0f;
    msg.MagY = 5.0f;
    msg.MagZ = 6.0f;
    msg.AccelX = 7.0f;
    msg.AccelY = 8.0f;
    msg.AccelZ = 9.0f;
    msg.GyroX = 10.0f;
    msg.GyroY = 11.0f;
    msg.GyroZ = 12.0f; // Değerleri random olarak atadım.

    char message[256];
    snprintf(message, sizeof(message), "%s%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f%s", packageStartDelimiter, packageHeader, msg.yaw, msg.pitch, msg.roll, msg.MagX, msg.MagY, msg.MagZ, msg.AccelX, msg.AccelY, msg.AccelZ, msg.GyroX, msg.GyroY, msg.GyroZ, packageTail);
    printf("%s\n", message); // Checksum hesaplanmadan gönderilecek komut

    char *str = "VNWRG,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0";
    unsigned char checksum = calc_cksum(str, strlen(str)); // checksum hesaplandı
    char finalmessage[256];
    snprintf(finalmessage, sizeof(finalmessage), "%s%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f%s%02X", packageStartDelimiter, packageHeader, msg.yaw, msg.pitch, msg.roll, msg.MagX, msg.MagY, msg.MagZ, msg.AccelX, msg.AccelY, msg.AccelZ, msg.GyroX, msg.GyroY, msg.GyroZ, packageTail, checksum);
    printf("%s\n", finalmessage); // Checksum hesaplanarak gönderilecek komut

    time_t start_time, current_time;
    double elapsed_time;
    time(&start_time);

    while (1)
    {
        time(&current_time);
        elapsed_time = difftime(current_time, start_time);
        if (elapsed_time >= 1.0)
        {
            write(hSerial, finalmessage);
            time(&start_time);
        }
        read(hSerial, readBuffer);
    }
    CloseHandle(hSerial); // Seri portu kapattım.
    return 0;
}
