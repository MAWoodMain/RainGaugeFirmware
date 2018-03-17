#ifndef BACKUP_REG_HPP
#define BACKUP_REG_HPP

#include "RTClock.h"
#define BKP_REG_BASE  (uint32_t *)(0x40006C00 +0x04)

void backupRegInit(void);
int readBackupReg(int registerNumber);
void writeBackupReg(int registerNumber,int value);

#endif
