#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "cksum.h"
#include "write.h"

#define device_name "VN-100T-CR"
#define hardware_revision "7"
#define serial_number "0100061897"

char *regNumber;
char *payload = NULL;
int checksum;
char buffer[BUFFER_SIZE];
char *variable = NULL;
unsigned char received_cksum;
unsigned char calculated_cksum;
extern int new_frequency;
extern HANDLE hSerial;

/**
 * @brief This function is a parser for processing specific command messages in the form of "$<Command>*<Checksum>".
 *
 * The parser is designed to handle messages with specific formats like:
 * - Start with a '$' symbol
 * - Contain certain keywords like "VNWRG" or "VNRRG"
 * - Separate components using commas (e.g., "$VNRRG,01,VN-100T-CR*32")
 * - Ensure the message ends with a checksum (after a '*' symbol).
 *
 * The parser goes through different steps to:
 * 1. Validate the presence of '$' and '*'
 * 2. Ensure the message begins with '$'
 * 3. Check the message contains specific commands like "VNWRG" or "VNRRG"
 * 4. Split the message by commas and process the payload
 * 5. Verify the device ID and payload values
 * 6. Perform checksum validation to ensure data integrity.
 *
 * The checksum is calculated for all characters in the message except for the '$' symbol.
 * The parser supports different device responses based on command number (e.g., "VNRRG,01,VN-100T-CR").
 * It sends back a response with the correct command, device information, and checksum if everything is valid.
 *
 * @param msg The input message string to be parsed.
 *
 * Steps are implemented using a simple state machine with the `step` variable controlling the flow.
 * Error handling is implemented at each stage, and the function exits with an appropriate message
 * when a condition is not met.
 *
 * External functions used:
 * - `calc_cksum()`: Calculates checksum of the provided string (excluding '$')
 * - `write()`: Sends the response message (the actual write method depends on your platform, e.g., serial communication)
 *
 * Example of message format:
 * - Command: "$VNRRG,01,VN-100T-CR*32"
 * - Checksum: The checksum after '*' should match the calculated one.
 *
 * The parser supports:
 * - Validating message structure
 * - Handling specific commands (e.g., "VNRRG")
 * - Processing frequency values from the payload (e.g., "1", "2", "5", etc.)
 * - Handling device ID validation ("07")
 */

/*
void parser(const char *msg)
{
    char buffer[BUFFER_SIZE];
    step = 1; // Başlangıç adımı
    char *variable = NULL;
    char *payload = NULL;
    int checksum;
    while (step > 0)
    {
        switch (step)
        {
        case 1: // '$' ve '*' kontrolü
            if (strchr(msg, '$') && strchr(msg, '*'))
            {
                step = 2; // Bir sonraki adıma geç
            }
            else
            {
                printf("Error: '$' or '*' not found in the message.\n");
                step = -1; // Hata durumunda çık
            }
            break;

        case 2: // '$' işaretinin ilk karakter olup olmadığını kontrol et
            if (msg[0] == '$')
            {
                step = 3; // Bir sonraki adıma geç
            }
            else
            {
                printf("Error: '$' is not the first letter in the message.\n");
                step = -2; // Hata durumunda çık
            }
            break;

        case 3: // Mesajın "VNWRG" içerip içermediğini kontrol et
            if (strstr(msg, "VNWRG") != NULL)
            {
                step = 4; // Bir sonraki adıma geç
            }

            else if (strstr(msg, "VNRRG") != NULL)
            {
                sscanf(msg + 7, "%2d", &commandNumber); // VNRRG'den sonra gelen 2 basamaklı sayıyı al

                if (commandNumber == 1) // Gelen sayıya göre işlem yap
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, device_name);
                    checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    variable = device_name;
                    //$VNRRG,01,VN-100T-CR*32
                    step = 4; // Bir sonraki adıma geç
                }
                else if (commandNumber == 2)
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, hardware_revision);
                    checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    //  $VNRRG,02,7*6A
                    variable = hardware_revision;
                    step = 4; // Bir sonraki adıma geç
                }
                else if (commandNumber == 3)
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, serial_number);
                    checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    variable = serial_number;
                    //  $VNRRG,03,0100061897*5C
                    step = 4; // Bir sonraki adıma geç
                }
                else
                {
                    printf("Error: Command number is incorrect. Expected '01', '02' or '03'.\n");
                    step = -3; // Hata durumunda çık
                }
                break;
            }
            else
            {
                printf("Message does not contain 'VNWRG' or 'VNRRG'. Ignoring...\n");
                step = -3; // Hata durumunda çık
            }
            break;

        case 4:
        { // Mesajın ',' ile ayrıldığını kontrol et ve ayrılmış parçalara böl

            strncpy(buffer, msg, sizeof(buffer) - 1); // Gelen mesajı buffer'a kopyala
            buffer[sizeof(buffer) - 1] = '\0';        // Güvenlik için null-terminator ekle

            if (strchr(buffer, ',') != NULL)
            {
                for (char *token = strtok(buffer, ","); token != NULL; token = strtok(NULL, ","))
                    ;     // null statement
                step = 5; // Bir sonraki adıma geç
            }
            else
            {
                printf("Error: Message is not separated by ','\n");
                step = -4; // Hata durumunda çık
            }
            break;
        }
        case 5:
        {                                          // Cihaz ID'sinin "07" olup olmadığını kontrol et
            char *buffer = strdup(msg);            // Mesajın bir kopyasını al
            char *token = strtok(buffer, ",");     // İlk token (VNWRG)
            token = strtok(NULL, ",");             // İkinci token (cihaz ID)
            char *secondtoken = strtok(NULL, ","); // Üçüncü token (yük), yani payload
            if ((strstr(msg, "VNWRG") != NULL) && (token != NULL) && (strcmp(token, "07") == 0))
            {
                if (secondtoken != NULL)
                {
                    // Yük kısmı yıldız işaretine kadar olan kısmı al
                    payload = secondtoken;
                    char *asterisk_pos = strchr(payload, '*');

                    if (asterisk_pos != NULL)
                    {
                        *asterisk_pos = '\0'; // Yıldızdan önceki kısmı al
                    }

                    // Payload'ı kontrol et
                    if (strcmp(payload, "1") == 0 || strcmp(payload, "2") == 0 || strcmp(payload, "5") == 0 || strcmp(payload, "10") == 0 || strcmp(payload, "40") == 0)
                    {
                        new_frequency = atoi(payload);
                        step = 6; // Bir sonraki adıma geç
                    }
                    else
                    {
                        printf("Error: Payload is incorrect. Expected '40'.\n");
                        step = -5; // Hata durumunda çık
                    }
                }
                else
                {
                    printf("Error: Payload token not found.\n");
                    step = -6; // Hata durumunda çık
                }
            }
            else if (strstr(msg, "VNRRG") != NULL)
            {
                step = 6; // Bir sonraki adıma geç
            }
            else
            {
                printf("Error: Device ID is incorrect. Expected '07'.\n");
                step = -5; // Hata durumunda çık
            }
            break;
        }

        case 6:
        {                                          // Mesajın checksum kontrolü
            char *asterisk_pos = strchr(msg, '*'); // '*' işaretinden önceki kısmı al (main kısmı)
            if (asterisk_pos == NULL)
            {
                printf("Error: '*' not found in the message.\n");
                step = -6; // Hata durumunda çık
                break;
            }

            size_t length = asterisk_pos - msg;
            // char main_part[256] yerine yer tasarrufu için buffer kullanıldı;
            strncpy(buffer, msg + 1, length - 1); // '$' işareti atlanacak
            buffer[length - 1] = '\0';            // Null-terminate

            char *cksum_part = asterisk_pos + 1;                                        // * sonrası kısmı (cksum kısmı)
            unsigned char received_cksum = (unsigned char)strtol(cksum_part, NULL, 16); // 16'lık sayı olarak parse et
            unsigned char calculated_cksum = calc_cksum(buffer, strlen(buffer));

            if (calculated_cksum == received_cksum)
            {
                if (strstr(msg, "VNRRG") != NULL)
                {
                    snprintf(buffer, sizeof(buffer), "$VNRRG,0%d,%s*%02X", commandNumber, variable, checksum);
                    write(hSerial, buffer);
                    step = 0;
                }
                else
                {
                    step = 0;                     // Tüm adımlar tamamlandı
                    printf("Message: %s\n", msg); // Mesajın tamamını yazdır
                    new_frequency = atoi(payload);
                }
            }
            else
            {
                printf("Checksum does not match. Calculated: %02X, Received: %02X\n", calculated_cksum, received_cksum);
                step = -6; // Hata durumunda çık
            }
            break;
        }
        default:
            printf("Invalid step. Exiting.\n");
            step = -1; // Hata durumunda çık
            break;
        }
    }
}

*/

static bool validateMessage(const char *msg)
{
    if (strchr(msg, '$') && strchr(msg, '*'))
        ;
    else
    {
        printf("Error: '$' or '*' not found in the message.\n");
        return false;
    }

    if (msg[0] == '$')
        ;
    else
    {
        printf("Error: '$' is not the first letter in the message.\n");
        return false;
    }

    char *asterisk_pos = strchr(msg, '*'); // '*' işaretinden önceki kısmı al (main kısmı)
    if (asterisk_pos == NULL)
    {
        printf("Error: '*' not found in the message.\n");
        return false;
    }

    size_t length = asterisk_pos - msg;
    // char main_part[256] yerine yer tasarrufu için buffer kullanıldı;
    strncpy(buffer, msg + 1, length - 1);
    buffer[length - 1] = '\0'; // Null-terminate

    char *cksum_part = asterisk_pos + 1;                                        // * sonrası kısmı (cksum kısmı)
    unsigned char received_cksum = (unsigned char)strtol(cksum_part, NULL, 16); // 16'lık sayı olarak parse et
    unsigned char calculated_cksum = calc_cksum(buffer, strlen(buffer));
    if (calculated_cksum == received_cksum)
        ;
    else
    {
        printf("Checksum does not match. Calculated: %02X, Received: %02X\n", calculated_cksum, received_cksum);
        return false;
    }
    strncpy(buffer, msg, sizeof(buffer) - 1); // Gelen mesajı buffer'a kopyala
    buffer[sizeof(buffer) - 1] = '\0';        // Güvenlik için null-terminator ekle

    if (strchr(buffer, ',') != NULL)
    {
        for (char *token = strtok(buffer, ","); token != NULL; token = strtok(NULL, ","))
            ; // null statement
    }
    else
    {
        printf("Error: Message is not separated by ','\n");
        return false;
    }
    return true;
}

char *extractRegiserNumber(const char *msg)
{
    static char regNumber[3];          // 2 karakter + null sonlandırıcı
    sscanf(msg + 7, "%2s", regNumber); // VNRRG'den sonra gelen 2 basamaklı sayıyı al
    return regNumber;
}

static void parserRegisterCommand(const char *msg)
{
    regNumber = extractRegiserNumber(msg); // 01
    if (strcmp(regNumber, "01") == 0)
    {
        snprintf(buffer, sizeof(buffer), "VNRRG,%s,%s", regNumber, device_name);
        checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
        variable = device_name;
    }
    else if (strcmp(regNumber, "02") == 0)
    {
        snprintf(buffer, sizeof(buffer), "VNRRG,%s,%s", regNumber, hardware_revision);
        checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
        variable = hardware_revision;
    }
    else if (strcmp(regNumber, "03") == 0)
    {
        snprintf(buffer, sizeof(buffer), "VNRRG,%s,%s", regNumber, serial_number);
        checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
        variable = serial_number;
    }
}

char *extractHeader(const char *msg)
{
    static char header[6];
    strncpy(header, msg, 6);
    header[6] = '\0';
    return header;
}

void parserYPRcommand(const char *msg)
{
    char *buffer = strdup(msg);            // Mesajın bir kopyasını al
    char *token = strtok(buffer, ",");     // İlk token (VNWRG)
    token = strtok(NULL, ",");             // İkinci token (cihaz ID)
    char *secondtoken = strtok(NULL, ","); // Üçüncü token (yük), yani payload
    if ((token != NULL) && (strcmp(token, "07") == 0))
    {
        if (secondtoken != NULL)
        {
            // Yük kısmı yıldız işaretine kadar olan kısmı al
            payload = secondtoken;
            char *asterisk_pos = strchr(payload, '*');
            if (asterisk_pos != NULL)
            {
                *asterisk_pos = '\0'; // Yıldızdan önceki kısmı al
            }
            // Payload'ı kontrol et
            if (strcmp(payload, "1") == 0 || strcmp(payload, "2") == 0 || strcmp(payload, "5") == 0 || strcmp(payload, "10") == 0 || strcmp(payload, "40") == 0)
            {
                new_frequency = atoi(payload);
                write(hSerial, msg);
            }
            else
            {
                printf("Error: Payload is incorrect. Expected '40'.\n");
            }
        }
        else
        {
            printf("Error: Payload token not found.\n");
        }
    }
    else
    {
        printf("Error: Device ID is incorrect. Expected '07'.\n");
    }
}

void parser(const char *msg)
{
    if (validateMessage(msg) == true)
    {
        char *header = extractHeader(msg);
        if (strcmp(header, "$VNRRG") == 0)
        {
            parserRegisterCommand(msg);
            snprintf(buffer, sizeof(buffer), "$VNRRG,%s,%s*%02X", regNumber, variable, checksum);
            write(hSerial, buffer);
        }
        else if (strcmp(header, "$VNWRG") == 0)
        {
            parserYPRcommand(msg);
            printf("Message: %s\n", msg); // Mesajın tamamını yazdır
        }
        else
        {
            printf("Message does not contain 'VNWRG' or 'VNRRG'. Ignoring...\n");
        }
    }
}