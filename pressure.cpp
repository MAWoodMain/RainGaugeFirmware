#include "pressure.hpp"

void initPressure()
{
  Wire.begin();
}

void readSensor(unsigned char* arr)
{
    Wire.requestFrom(0x28, 4);    // request 6 bytes from slave device #0x28

    for(int i =0; i<4; i++) { // slave may send less than requested
      arr[i] = Wire.read(); // receive a byte as character
    }
}

bool isStale(unsigned char* arr)
{
  return (arr[0] & 0xC0) != 0x00;
}

int getPressure(unsigned char* arr)
{
  return ((arr[0] & 0xff) << 8) | (arr[1] & 0xff);
}

int getTemperature(unsigned char* arr)
{
  return (arr[2] << 3)|(arr[3] >> 5);
}

int adjustPressure(int rawPressure)
{
    int pressure = constrain(rawPressure, 1639, 3276);
    pressure = map(pressure, 1639, 3276, 0, 1023);
    return pressure;
}
int getAverageRawPressure()
{
  uint8_t data[4];
  int pressure = 0;
  for(int i = 0; i < 10; i++)
  {
    do
    {
      readSensor(&data[0]);
    } while(isStale(&data[0]));
    pressure += getPressure(&data[0]);
  }
  return pressure/10;  
}
