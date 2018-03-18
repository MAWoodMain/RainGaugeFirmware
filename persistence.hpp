#ifndef PERSISTENCE_HPP
#define PERSISTENCE_HPP

#include "backupReg.hpp"
#include "Arduino.h"
#define STATE_LOCATION 15
#define TRANSMIT_STATE 14


int getStateNumber(void);
bool isTransmitState(void);
void storeReading(int reading);
void incrementState(void);

void getReadings(int readings[]);

#endif
