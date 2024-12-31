#include "parser.h"
#include "cksum.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// BRİEF EKLE
void parser(const char *msg)
{

    // "$" ve "*" karakterlerini kontrol et
    if (strchr(msg, '$') && strchr(msg, '*'))
    {
        // Eğer hem "$" hem de "*" varsa, bir sonraki adıma geç
        if (msg[0] == '$')
        {
            char *asterisk_pos = strchr(msg, '*'); // '*' işaretinden önceki kısmı al (main kısmı)
            size_t length = asterisk_pos - msg;
            char *main_part = (char *)malloc(length + 1);
            strncpy(main_part, msg + 1, length - 1); // '$' işareti atlanacak
            main_part[length - 1] = '\0';            // Null-terminate

            char *cksum_part = asterisk_pos + 1; // * sonrası kısmı (cksum kısmı)
            unsigned char received_cksum = (unsigned char)strtol(cksum_part, NULL, 16);  // 16'lık sayı olarak parse et
            if ( calc_cksum(cksum_part,strlen(cksum_part)) == received_cksum)
            {
                printf("its ok");
            }
            else
            {
                printf("Checksum does not match. Calculated: %02X, Received: %02X\n",calc_cksum(cksum_part,strlen(cksum_part)), received_cksum);
            }
        }
        else
        {
            printf("Error: '$' is not the first letter in the message.\n");
        }
    }
    else
    {
        printf("Error: '$' or '*' not found in the message.\n");
    }
    // 1. Check if message has "$" and "*"
    // 1.1 Check if "$" is the first letter
    // 2. $msg*CRC
    // 2.1 extract the msg
    // 2.2 extract the CRC
    // 3. Check if CRC is valid (of msg) error:

    // 4. Parse the message
    // 4.1 Parse the header "VNWRG" (extract with ",")
    //      4.2 Parse the message id = 07
    //          4.3 Parse the payload = variable (variable = period)
}