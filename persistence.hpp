#ifndef PERSISTENCE_HPP
#define PERSISTENCE_HPP

#include "backupReg.hpp"
#include "Arduino.h"
#define STATE_LOCATION 0
#define TRANSMIT_STATE 16


int getStateNumber(void);
bool isTransmitState(void);
void storeReading(int reading);
void incrementState(void);
void storeTenBitValue(int position, int value);
int getTenBitValue(int position);

#endif
