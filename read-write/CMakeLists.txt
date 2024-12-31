cmake_minimum_required(VERSION 3.5.0)
project(myProject VERSION 0.1.0 LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/functions)

add_executable(myProject main.c functions/cksum.c functions/parser.c functions/read.c functions/serial_port.c functions/write.c)
