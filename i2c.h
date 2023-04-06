#ifndef I2C_H
# define  I2C_H

# include "mpc23017.h"

enum { Out = 0, Inp = 1 };

extern byte  tglTestEn;

void i2cWrite    (byte chip, byte port, byte val);
void i2cWriteBit (byte adr,  bool val);
void i2cWritePortBit (byte adr, byte port, bool val);
byte i2cRead     (byte chip, byte port);
bool i2cReadBit  (byte adr);
void i2cReset    (void);
void i2cCfg      (void);
void i2cDump     (byte chip);
void i2cList     (void);
void i2cScan     (void);
void test        (int chip);
void tglTest     (void);
void bitTgl      (byte chip, byte reg, byte bit);
void i2cInit     (void);

#endif
