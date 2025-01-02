# Docklight Serial Port Communication Project With VN-100

This project is a program that communicates with Docklight via a serial port using uart protocol, reading, writing, and validating sensor data. This project uses com12 and com 13 as serial ports.
The program parses incoming messages, validates them, and returns appropriate responses based on the received messages. 
Additionally, the program generates random sensor data at a 1Hz rate if no messages are received from Docklight.

## Prerequisites:
Ensure you have a Windows environment.
You need a C compiler like GCC or an IDE like Code::Blocks or Visual Studio.
The program requires the windows.h library for serial port communication.

## Files
-main.c: Contains the main logic of the program. It handles message validation, frequency adjustments, and checksum calculation.
-write.c: Functions for writing data to the serial port.
-read.c: Functions for reading data from the serial port.
-parser.c: Functions for parsing received data.
-cksum.c: Functions for checksum calculation.
-serial_port.c: Functions to handle the initialization and management of the serial port.

## Project Overview
This program processes messages received from Docklight as follows:

## Message Validation:
If the message is correctly formatted and starts with "$VNWRG", it sends messages at the desired frequency and echoes the received message back to Docklight.
If the message starts with "$VNRRG", it processes the message, retrieves information based on the specified register, and sends only that information to Docklight without echoing the entire message.

## Error Handling:
If the received message is incorrectly formatted, an error message is returned.

## No Message Received:
If no message is received from Docklight, the program generates and sends a message with randomly changing sensor values (yaw, pitch, roll, MagX, MagY, MagZ, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ) at a 1Hz rate.

## Checksum Calculation:
A checksum is calculated for every message, and it is appended to the message.

## Key Features
Message Validation and Processing: Messages must follow the format $(header),(data)*. If the message is invalid, an error message is sent.
$VNWRG and $VNRRG Headers: These headers are processed separately. Messages starting with $VNWRG are echoed back, while messages starting with $VNRRG are processed according to the specified register, and only relevant data is sent back to Docklight.
Random Sensor Data Generation: If no messages are received from Docklight, random sensor data messages are sent at 1Hz, containing values for yaw, pitch, roll, MagX, MagY, MagZ, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ.
Checksum Calculation: For all messages, a checksum is calculated and appended to the end of the message.

## Commands Sent From Docklight

![image](https://github.com/user-attachments/assets/cdb4ebfe-e186-42ea-b65b-9a351b9ecf22)


# Acknowledgements
This project uses Windows API functions for serial port communication.
The checksum calculation algorithm is based on standard methods for ensuring data integrity in communication protocols.
