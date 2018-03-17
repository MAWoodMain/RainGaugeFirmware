#include "persistence.hpp"

int getStateNumber()
{
  getTenBitValue(STATE_LOCATION);
}

bool isTransmitState()
{
  return getStateNumber() == TRANSMIT_STATE;
}

void storeReading(int reading)
{
  int state = getStateNumber();
  if(state != TRANSMIT_STATE)
  {
    storeTenBitValue(state, reading);
  }
}

void incrementState()
{
  storeTenBitValue(STATE_LOCATION, (getStateNumber() + 1) % 17);
}

void getReadings(int readings[])
{
  for(int i=0; i<16; i++)
  {
    readings[i] = getTenBitValue(i);
  }
}


