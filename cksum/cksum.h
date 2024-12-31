#ifndef CKSUM_H
#define CKSUM_H

#define NUMBER_OF_RANDOM_COEFFICIENTS 12
#define BUFFER_SIZE                   256

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

unsigned char calc_cksum(const char data[], unsigned int length);

void initializeRandomCoefficients(void);

void set_frequency(int value);

int get_new_frequency();


unsigned char update_checksum(YMR_package_t *msg);
#endif /* CKSUM_H */