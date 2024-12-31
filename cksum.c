#include "cksum.h"
#include <stdio.h>
/**
 * @brief Calculates a checksum for a given data array.
 *
 * This function computes a simple checksum by XOR-ing each byte of the input data array.
 * The checksum is useful for error detection in data transmission. 
 * The result is a single byte that represents the cumulative XOR of all input bytes.
 *
 * @param data A pointer to the data array for which the checksum is calculated.
 * @param length The length of the data array.
 *
 * @return The calculated checksum as an unsigned char.
 */

unsigned char calc_cksum(const char data[], unsigned int length){
    unsigned int i;
    unsigned char cksum = 0;
    for (i = 0; i < length; i++)
    {
        cksum ^= data[i];
    }
    return cksum;
}