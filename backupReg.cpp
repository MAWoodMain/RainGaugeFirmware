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

void storeTenBitValue(int position, int value)
{
  int regNum = position>7 ? 5:0;
  int reg = 0;
  int upper = 0;
  int lower = 0;
  value &= 0x03ff;
  switch(position % 8)
  {
    case 0:
      regNum += 0;
      reg = readBackupReg(regNum);
      reg &= ~0x03ff;
      reg |= value;
      writeBackupReg(regNum, reg);
      break;
      
    case 1:
      regNum += 0;
      lower = value << 10;
      upper = value >> 6;
      
      reg = readBackupReg(regNum);
      reg &= ~0xfc00;
      reg |= lower;
      writeBackupReg(regNum,reg);
      
      regNum++;
      reg = readBackupReg(regNum);
      reg &= ~0x000f;
      reg |= upper;
      writeBackupReg(regNum, reg);
      break;
      
    case 2:
      regNum += 1;
      reg = readBackupReg(regNum);
      reg &= ~0x3ff0;
      reg |= (value << 4);
      writeBackupReg(regNum, reg);
      break;
      
    case 3:
      regNum += 1;
      lower = value << 14;
      upper = value >> 2;
      
      reg = readBackupReg(regNum);
      reg &= ~0xc000;
      reg |= lower;
      writeBackupReg(regNum,reg);
      
      regNum++;
      reg = readBackupReg(regNum);
      reg &= ~0x00ff;
      reg |= upper;
      writeBackupReg(regNum, reg);
      break;
      
    case 4:
      regNum += 2;
      lower = value << 8;
      upper = value >> 8;
      
      reg = readBackupReg(regNum);
      reg &= ~0xff00;
      reg |= lower;
      writeBackupReg(regNum,reg);
      
      regNum++;
      reg = readBackupReg(regNum);
      reg &= ~0x0003;
      reg |= upper;
      writeBackupReg(regNum, reg);
      break;
      
    case 5:
      regNum += 3;
      lower = value << 2;
      
      reg = readBackupReg(regNum);
      reg &= ~0x0ffc;
      reg |= lower;
      writeBackupReg(regNum,reg);
      break;
      
    case 6:
      regNum += 3;
      lower = value << 12;
      upper = value >> 4;
      
      reg = readBackupReg(regNum);
      reg &= ~0xf000;
      reg |= lower;
      writeBackupReg(regNum,reg);
      
      regNum++;
      reg = readBackupReg(regNum);
      reg &= ~0x003f;
      reg |= upper;
      writeBackupReg(regNum, reg);
      break;
      
    case 7:
      regNum += 4;
      lower = value << 6;
      
      reg = readBackupReg(regNum);
      reg &= ~0xffc0;
      reg |= lower;
      writeBackupReg(regNum,reg);
      break;
  }
  //Serial1.println(upper, HEX);
  //Serial1.println(lower, HEX);
}

int getTenBitValue(int position)
{
  int regNum = position>7 ? 5:0;
  int value = 0;
  int upper = 0;
  int lower = 0;
  switch(position % 8)
  {
    case 0:
      regNum += 0;
      value = readBackupReg(regNum);
      value &= 0x03ff;
      break;
      
    case 1:
      regNum += 0;
      lower = readBackupReg(regNum);
      lower &= 0xfc00;
      regNum++;
      upper = readBackupReg(regNum);
      upper &= 0x000f;
      value = (lower >> 10) | (upper << 6);
      break;
      
    case 2:
      regNum += 1;
      value = readBackupReg(regNum) & 0x3ff0;
      value >>= 4;
      break;
      
    case 3:
      regNum += 1;
      lower = readBackupReg(regNum);
      lower &= 0xc000;
      regNum++;
      upper = readBackupReg(regNum);
      upper &= 0x00ff;
      value = (lower >> 14) | (upper << 2);
      break;
      
    case 4:
      regNum += 2;
      lower = readBackupReg(regNum);
      lower &= 0xff00;
      regNum++;
      upper = readBackupReg(regNum);
      upper &= 0x0003;
      value = (lower >> 8) | (upper << 8);
      break;
      
    case 5:
      regNum += 3;
      value = readBackupReg(regNum) & 0x0ffc;
      value >>= 2;
      break;
      
    case 6:
      regNum += 3;
      lower = readBackupReg(regNum);
      lower &= 0xf000;
      regNum++;
      upper = readBackupReg(regNum);
      upper &= 0x003f;
      value = (lower >> 12) | (upper << 4);
      break;
      
    case 7:
      regNum += 4;
      value = readBackupReg(regNum) & 0xffc0;
      value >>= 6;
      break;
  }
  //Serial1.println(upper, HEX);
  //Serial1.println(lower, HEX);
  return value;
}
