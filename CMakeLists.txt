cmake_minimum_required(VERSION 3.5.0)
project(myProject VERSION 0.1.0 LANGUAGES C)

include_directories(${CMAKE_SOURCE_DIR}/read-write)
include_directories(${CMAKE_SOURCE_DIR}/parser)
include_directories(${CMAKE_SOURCE_DIR}/cksum)
include_directories(${CMAKE_SOURCE_DIR}/serial-port)

add_executable(myProject main.c parser/parser.c read-write/read.c serial-port/serial_port.c read-write/write.c cksum/cksum.c)
