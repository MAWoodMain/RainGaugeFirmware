#include "payloadBuilder.hpp"

void reverse_array(unsigned char* arr, int array_length) {
    int i = array_length - 1;
    int j = 0;
    int temp = 0;

    while(i > j)
    {
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        i--;
        j++;
    }
}
void construct_block(unsigned char* arr, unsigned char block_id, unsigned char data_length, void * data) {
    arr[0] = block_id;
    arr[1] = data_length;
    memcpy(&arr[2], data, data_length);

    // Correct data endianness
    reverse_array(&arr[2], data_length);
}

#define BKP_REG_BASE  (uint32_t *)(0x40006C00 +0x04)
void construct_rain_block(unsigned char* arr)
{
  arr[0] = BLOB_BLOCK_ID;
  arr[1] = 19;
  for(int i=0; i<9; i++)
  {
    memcpy(&arr[2 + 2*i], BKP_REG_BASE + i, 2);
  }
  memcpy(&arr[2 + 18], BKP_REG_BASE + 9, 1);
  arr[21] = arr[2+19] & 0xf3;
}

void construct_segment_header(unsigned char* arr, unsigned char section_id) {
    arr[0] = SEGMENT_START;
    arr[1] = 0x00;
    arr[2] = section_id;
}

void construct_battery_segment(unsigned char* arr, int battery) {
    construct_segment_header(&arr[0], BATTERY_SEG_ID);
    construct_block(&arr[0 * (SHORT_BLOCK_LENGTH + 2) + 3], SHORT_BLOCK_ID, SHORT_BLOCK_LENGTH, &battery);
}

void construct_time_segment(unsigned char* arr, int time) {
    construct_segment_header(&arr[0], TIME_SEG_ID);
    construct_block(&arr[0 * (INT_BLOCK_LENGTH + 2) + 3], INT_BLOCK_ID, INT_BLOCK_LENGTH, &time);
}

void construct_rain_segment(unsigned char* arr) {
    construct_segment_header(&arr[0], RAIN_SEG_ID);
    construct_rain_block(&arr[3]);
}

int constructPayload(int battery, int time, unsigned char* arr)
{
    int packet_length = 2;
    arr[0] = PACKET_START_1;
    arr[1] = PACKET_START_2;

    if (time != 0) {
        construct_time_segment(&arr[packet_length], time);         // Size = 3 (segment header) + 6 * 1  (blocks) =  9 bytes
        packet_length += 9;
    }
    if (battery != 0) {
        construct_battery_segment(&arr[packet_length], battery);     // Size = 3 (segment header) + 4 * 1  (blocks) =  7 bytes
        packet_length += 7;
    }
    // 2 + 9 + 7 = 18 for header, time and battery 51-18 = 33 bytes left for rain data

    construct_rain_segment(&arr[packet_length]);
    packet_length += (3 + 2 + 19); // 24
    return packet_length; // 42 bytes
}

