#include "persistence.hpp"

int getStateNumber()
{
  return getTenBitValue(15);
}

bool isTransmitState()
{
  return getStateNumber() >= TRANSMIT_STATE;
}

void storeReading(int reading)
{
  int state = getStateNumber();
  Serial1.print("Storing ");
  Serial1.print(reading);
  Serial1.print(" in ");
  Serial1.println(state);
  storeTenBitValue(state, reading);
}

void incrementState()
{
  int state = getStateNumber();
  state++;
  state %= 15;
  storeTenBitValue(15, state);
}

void getReadings(int readings[])
{
  for(int i=0; i<16; i++)
  {
    readings[i] = getTenBitValue(i);
  }
}


