#include <stdio.h>
#include <string.h>
#include <windows.h> //Gerekli kütüphaneleri ekledim
#include <math.h>    // Include math library for sin function
#include <time.h>
#include "read-write/write.h"
#include "read-write/read.h"
#include "cksum/cksum.h"
#include "serial-port/serial_port.h"

extern HANDLE hSerial;
extern double randomCoefficients[NUMBER_OF_RANDOM_COEFFICIENTS];
char readBuffer[BUFFER_SIZE];
extern int frequency;
extern int new_frequency;
char *packageStartDelimiter = "$";
char *packageHeader = "VNYMR";
char *packageTail = "*";
YMR_package_t msg;

int main()
{
    serial_port();
    char message[BUFFER_SIZE];
    initializeRandomCoefficients();
    clock_t start_time, current_time;
    double elapsed_time;
    start_time = clock();

    while (1)
    {
        double current_time = (double)time(NULL);

        // for(int i = 0; i < 12; i++)
        // {
        //     size_t offset = offsetof(YMR_package_t, yaw) + i * sizeof(float);
        // }

        msg.yaw = sin(current_time * randomCoefficients[0]);
        msg.pitch = sin(current_time * randomCoefficients[1]);
        msg.roll = sin(current_time * randomCoefficients[2]);
        msg.MagX = sin(current_time * randomCoefficients[3]);
        msg.MagY = sin(current_time * randomCoefficients[4]);
        msg.MagZ = sin(current_time * randomCoefficients[5]);
        msg.AccelX = sin(current_time * randomCoefficients[6]);
        msg.AccelY = sin(current_time * randomCoefficients[7]);
        msg.AccelZ = sin(current_time * randomCoefficients[8]);
        msg.GyroX = sin(current_time * randomCoefficients[9]);
        msg.GyroY = sin(current_time * randomCoefficients[10]);
        msg.GyroZ = sin(current_time * randomCoefficients[11]);

        unsigned char checksum = update_checksum(&msg);
        snprintf(message, sizeof(message), "%s%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f%s%02X",
                 packageStartDelimiter, packageHeader, msg.yaw, msg.pitch, msg.roll, msg.MagX, msg.MagY, msg.MagZ, msg.AccelX, msg.AccelY,
                 msg.AccelZ, msg.GyroX, msg.GyroY, msg.GyroZ, packageTail, checksum);

        current_time = clock();
        elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC;

        double period = 1.0 / ((double)frequency);
        if ((elapsed_time) >= period)
        {
            printf("%s\n", message); 
            write(hSerial, message);
            start_time = clock();
        }
        read(hSerial, readBuffer, sizeof(readBuffer));
    }

    CloseHandle(hSerial); // Seri portu kapattım.
    return 0;
}
