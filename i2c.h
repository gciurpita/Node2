#ifndef I2C_H
# define  I2C_H

extern byte  tglTestEn;

void i2cWrite (int chip, int port, byte val);
byte i2cRead  (int chip, int port);
void i2cCfg   (void);
void i2cList  (void);
void i2cScan  (void);
void test     (int chip);
void tglTest  (void);
void bitTgl   (byte chip, byte reg, byte bit);
void i2cInit  (void);

#endif
