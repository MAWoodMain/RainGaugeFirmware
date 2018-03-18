#ifndef PAYLOAD_BUILDER_HPP
#define PAYLOAD_BUILDER_HPP

#include "Arduino.h"

#define PACKET_START_1          0xDA
#define PACKET_START_2          0x01

#define SEGMENT_START           0xAA

#define BATTERY_SEG_ID          0x01
#define TIME_SEG_ID             0x04
#define RAIN_SEG_ID             0x0f

#define BLOB_BLOCK_ID           0x00

#define BYTE_BLOCK_ID           0x01
#define SHORT_BLOCK_ID          0x02
#define INT_BLOCK_ID            0x03
#define LONG_BLOCK_ID           0x04

#define FLOAT_BLOCK_ID          0x05
#define DOUBLE_BLOCK_ID         0x06

#define BYTE_BLOCK_LENGTH       0x01
#define SHORT_BLOCK_LENGTH      0x02
#define INT_BLOCK_LENGTH        0x04
#define LONG_BLOCK_LENGTH       0x08

#define FLOAT_BLOCK_LENGTH      0x04
#define DOUBLE_BLOCK_LENGTH     0x08

int constructPayload(int battery, int time, float temp, unsigned char* arr);

#endif
