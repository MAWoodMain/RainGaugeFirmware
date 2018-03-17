#include "tx_counter.hpp"

int read_counter() {
    int cnt = 0;
    uint16 readVal = 0;

    EEPROM.read(H_BYTES, &readVal);
    cnt |= readVal;
    cnt <<= 16;
    EEPROM.read(L_BYTES, &readVal);
    cnt |= readVal;

    return cnt;
}

int set_counter(int cnt) {
    EEPROM.write(H_BYTES, (cnt >> 16) & 0xFFFF);
    EEPROM.write(L_BYTES, (cnt >> 0 ) & 0xFFFF);
}

void increment_counter() {
    set_counter(read_counter() + 1);
}
