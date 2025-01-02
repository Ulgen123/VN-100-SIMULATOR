#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "cksum.h"
#include "write.h"

#define device_name "VN-100T-CR"
#define hardware_revision "7"
#define serial_number "0100061897"

extern int new_frequency;
extern HANDLE hSerial;

/**
 * @brief Parses and validates a received message according to specific rules.
 *
 * The `parser` function analyzes a message, performs several validation steps,
 * extracts relevant data, and triggers specific actions based on the content of the message.
 *
 * ### Functionality:
 * - Checks for the presence of mandatory symbols (`$`, `*`) and ensures the message starts with `$`.
 * - Verifies the message type (`VNWRG` or `VNRRG`) and handles specific commands.
 * - Extracts and validates the command number and payload based on the message content.
 * - Performs checksum verification to ensure message integrity.
 * - Updates the system frequency if a valid payload is received.
 * - Handles errors at each step and logs descriptive messages.
 *
 * ### Steps:
 * 1. Check for `$` and `*` symbols.
 * 2. Ensure the message starts with `$`.
 * 3. Handle `VNRRG` or `VNWRG` messages with specific logic for commands `01`, `02`, and `03`.
 * 4. Parse the payload if the device ID matches `07`.
 * 5. Validate and update the system frequency based on the payload.
 * 6. Calculate and compare the checksum to ensure data integrity.
 *
 * ### Parameters:
 * @param msg  The message string to be parsed. Assumes the string is null-terminated.
 *
 * ### Notes:
 * - The function uses a state machine approach to manage the parsing process.
 * - Error states are handled gracefully, with appropriate messages logged.
 * - Commands `01`, `02`, and `03` trigger responses using the `write` function.
 *
 * ### Error Handling:
 * - Logs errors for missing symbols, invalid commands, or checksum mismatches.
 * - Stops further processing when an error occurs.
 *
 * ### Expected Message Format:
 * - Format: `$<Command>,<DeviceID>,<Payload>*<Checksum>`
 *
 * ### Dependencies:
 * - `calc_cksum`: Calculates the checksum of a message.
 * - `write`: Sends data through a communication channel.
 */
void parser(const char *msg)
{
    char buffer[BUFFER_SIZE];
    int step = 1; // Başlangıç adımı

    char *payload = NULL;
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
                int commandNumber;
                sscanf(msg + 7, "%2d", &commandNumber); // VNRRG'den sonra gelen 2 basamaklı sayıyı al

                if (commandNumber == 1) // Gelen sayıya göre işlem yap
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, device_name);
                    int checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    snprintf(buffer, sizeof(buffer), "$VNRRG,0%d,%s*%02X", commandNumber, device_name, checksum);
                    write(hSerial, buffer);
                    //$VNRRG,01,VN-100T-CR*32
                    step = 4; // Bir sonraki adıma geç
                }
                else if (commandNumber == 2)
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, hardware_revision);
                    int checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    snprintf(buffer, sizeof(buffer), "$VNRRG,0%d,%s*%02X", commandNumber, hardware_revision, checksum);
                    write(hSerial, buffer);
                    // $VNRRG,02,7*6A
                    step = 4; // Bir sonraki adıma geç
                }
                else if (commandNumber == 3)
                {
                    snprintf(buffer, sizeof(buffer), "VNRRG,0%d,%s", commandNumber, serial_number);
                    int checksum = calc_cksum(buffer, strlen(buffer)); // '$' hariç tüm karakterler
                    snprintf(buffer, sizeof(buffer), "$VNRRG,0%d,%s*%02X", commandNumber, serial_number, checksum);
                    write(hSerial, buffer);
                    // $VNRRG,03,0100061897*5C
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
                step = 6;   // Bir sonraki adıma geç
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
                step = 0;                     // Tüm adımlar tamamlandı
                printf("Message: %s\n", msg); // Mesajın tamamını yazdır
                new_frequency = atoi(payload);
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