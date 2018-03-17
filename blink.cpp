#include "blink.hpp"
void blinkInit()
{
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);
}
void shortBlink()
{
    digitalWrite(PC13, LOW);
    delay(100);
    digitalWrite(PC13, HIGH);
}

void standardBlink()
{
    digitalWrite(PC13, LOW);
    delay(750);
    digitalWrite(PC13, HIGH);
}

void longBlink()
{
    digitalWrite(PC13, LOW);
    delay(1500);
    digitalWrite(PC13, HIGH);
}

void doubleBlink()
{
    digitalWrite(PC13, LOW);
    delay(150);
    digitalWrite(PC13, HIGH);
    delay(150);
    digitalWrite(PC13, LOW);
    delay(150);
    digitalWrite(PC13, HIGH);
}

