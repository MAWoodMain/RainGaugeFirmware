#include "backupReg.hpp"

void backupRegInit()
{
    rtc_init(RTCSEL_LSE);
}

int readBackupReg(int registerNumber)
{
  if (registerNumber>9)
  {
    registerNumber+=5;// skip over BKP_RTCCR,BKP_CR,BKP_CSR and 2 x Reserved registers
  }
  return *(BKP_REG_BASE+registerNumber)&0xffff;
}
  
void writeBackupReg(int registerNumber,int value)
{
  if (registerNumber>9)
  {
    registerNumber+=5;// skip over BKP_RTCCR,BKP_CR,BKP_CSR and 2 x Reserved registers
  }
  *(BKP_REG_BASE+registerNumber)=value&0xffff;
}

