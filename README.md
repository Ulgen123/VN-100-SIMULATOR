# Docklight Serial Port Communication Project With VN-100
This project is a program that communicates with Docklight via a serial port using uart protocol, reading, writing, and validating sensor data.
This project uses com12 and com 13 as serial ports. The program parses incoming messages, validates them, and returns appropriate responses
based on the received messages. Additionally, the program generates random sensor data at a 1Hz rate if no messages are received from Docklight.
## Prerequisites:
Ensure you have a Windows environment. You need a C compiler like GCC or an IDE like Code::Blocks or Visual Studio. The program requires
the windows.h library for serial port communication.
## Files
- main.c: Contains the main logic of the program. It handles message validation, frequency adjustments, and checksum calculation.
- write.c: Functions for writing data to the serial port.
- read.c: Functions for reading data from the serial port.
- parser.c: Functions for parsing received data.
- cksum.c: Functions for checksum calculation.
- serial_port.c: Functions to handle the initialization and management of the serial port.
## Project Overview
This program processes messages received from Docklight as follows:
## Message Validation:
If the message is correctly formatted and starts with "$VNWRG", it sends messages at the desired frequency and echoes the received message
back to Docklight. If the message starts with "$VNRRG", it processes the message, retrieves information based on the specified register, and
sends only that information to Docklight without echoing the entire message.
## Error Handling:
If the received message is incorrectly formatted, an error message is returned.

