#include <stdio.h>
#include "cksum.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

double randomCoefficients[NUMBER_OF_RANDOM_COEFFICIENTS]; // 12 elemanlı bir dizi tanımladım
extern int frequency;
extern int new_frequency;

/**
 * @brief Sets the system frequency to a new positive value.
 *
 * This function updates the global `frequency` variable to the specified value
 * if the value is positive. Negative or zero values are ignored.
 *
 * @param value The new frequency value to set. Must be positive.
 */


void set_frequency(int value)
{
    if (value > 0)
    { // Frequency negatif olamaz
        frequency = value;
    }
    else
    {
        //printf("Frequency must be a positive value.\n");
    }
}

/**
 * @brief Retrieves the new frequency value.
 *
 * This function returns the value of the global `new_frequency` variable.
 *
 * @return The current value of `new_frequency`.
 */

int get_new_frequency()
{
    return new_frequency;
}

/**
 * @brief Calculates a checksum for a given data array using XOR operation.
 *
 * This function computes a checksum for the provided data by performing an XOR
 * operation over all bytes in the array.
 *
 * @param data   A pointer to the data array.
 * @param length The number of bytes in the data array.
 *
 * @return The calculated checksum as an unsigned char.
 */

unsigned char calc_cksum(const char data[], unsigned int length)
{
    unsigned char cksum = 0;

    for (unsigned int data_index = 0; data_index < length; data_index++)
    {
        cksum ^= data[data_index];
    }
    return cksum;
}

/**
 * @brief Initializes an array of random coefficients with values between 0.1 and 1.0.
 *
 * This function generates random coefficients and populates the `randomCoefficients` array.
 * The random values are derived using the `rand` function, seeded with the current time.
 *
 * @note The array size is defined by the `NUMBER_OF_RANDOM_COEFFICIENTS` macro.
 */

void initializeRandomCoefficients()
{
    srand(time(NULL)); // Rastgelelik için başlangıç tohumunu ayarladım
    for (int rand_index = 0; rand_index < NUMBER_OF_RANDOM_COEFFICIENTS; rand_index++)
    {
        randomCoefficients[rand_index] = (rand() % 10 + 1) / 10.0; // 0.1 ile 1.0 arasında rastgele bir katsayı
    }
}

/**
 * @brief Updates the checksum of a YMR package message structure.
 *
 * This function creates a string representation of the given `YMR_package_t` message,
 * calculates its checksum using the `calc_cksum` function, and returns the updated checksum.
 *
 * @param msg A pointer to the `YMR_package_t` message structure containing sensor data.
 *
 * @return The updated checksum as an unsigned char.
 *
 * ### Example Message Format:
 * ```
 * "VNYMR,<yaw>,<pitch>,<roll>,<MagX>,<MagY>,<MagZ>,<AccelX>,<AccelY>,<AccelZ>,<GyroX>,<GyroY>,<GyroZ>"
 * ```
 */

unsigned char update_checksum(YMR_package_t *msg)
{
    // Create a string representation of the message
    char str[BUFFER_SIZE]; // String to hold the formatted message
    snprintf(str, sizeof(str), "VNYMR,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
             msg->yaw, msg->pitch, msg->roll, msg->MagX, msg->MagY, msg->MagZ,
             msg->AccelX, msg->AccelY, msg->AccelZ, msg->GyroX, msg->GyroY, msg->GyroZ);
    // Calculate checksum of the updated string
    return calc_cksum(str, strlen(str));
}