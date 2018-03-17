#ifndef PRESSURE_HPP
#define PRESSURE_HPP

#include<Wire.h>

void initPressure(void);
void readSensor(unsigned char* arr);
bool isStale(unsigned char* arr);
int getPressure(unsigned char* arr);
int getTemperature(unsigned char* arr);
int adjustPressure(int rawPressure);
int getAverageRawPressure();

#endif
