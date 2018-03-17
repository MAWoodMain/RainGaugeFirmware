#ifndef TX_COUNTER_HPP
#define TX_COUNTER_HPP

#include <EEPROM.h>

#define H_BYTES 0x0000
#define L_BYTES 0x0002

int read_counter(void);
int set_counter(int cnt);
void increment_counter(void);

#endif
