#include <Arduino.h>
#include "Wire.h"  // I2C operations

#include "i2c.h"
#include "node.h"
#include "mpc23017.h"

// -----------------------------------------------------------------------------
//      MPC    Uno  Mega  Esp32
//  SCK  12 <-  A5   21    22
//  SDA  13 <-  A4   20    21

#define MAX_CHIP    8

// I2C definitions

const int Chip20 = 0x20;
const int Chip21 = 0x21;
const int Chip22 = 0x22;
const int Chip23 = 0x23;
const int Chip24 = 0x24;
const int Chip25 = 0x25;

const int IoInp  = 0xFF;
const int IoOut  = 0x00;


byte chip    = Chip20;
byte port    = GPIOA;

// ---------------------------------------------------------
void i2cWrite (
    int     chip,
    int     port,
    byte    val )
{
    chip += chip < Chip20 ? Chip20 : 0;

    if (2 < dbg)  {
        Serial.print (F ("  i2cWrite: chip "));
        Serial.print (chip, HEX);
        Serial.print (", port ");
        Serial.print (port, HEX);
        Serial.print (", val ");
        Serial.println (val, HEX);
    }

    Wire.beginTransmission (chip);
    Wire.write (port);    
    Wire.write (val);    
    Wire.endTransmission ();
}

// ---------------------------------------------------------
byte
i2cRead (
    int     chip,
    int     port )
{
    if (8 & dbg)
        return 0xFF;

    chip += chip < Chip20 ? Chip20 : 0;

    Wire.beginTransmission (chip);
    Wire.write (port);    
    Wire.endTransmission ();

    Wire.requestFrom (chip, 1); //get 1 byte
    byte val =  Wire.read ();

    if (3 < dbg)  {
        Serial.print (F ("  i2cRead: chip "));
        Serial.print (chip, HEX);
        Serial.print (", port ");
        Serial.print (port, HEX);
        Serial.print (", val ");
        Serial.println (val, HEX);
    }

    return val;
}

// ---------------------------------------------------------
int nChip   = 0;

void i2cCfg (void)
{
    printf ("%s:\n", __func__);
    for (int n = 0; n < nChip; n++)  {

        i2cWrite (n, IODIRA, IoOut);
        if (IoOut != i2cRead (n, IODIRA))
            continue;

        i2cWrite (n, IODIRB, IoInp);
        if (IoInp != i2cRead (n, IODIRB))
            continue;

        i2cWrite (n, GPPUA,  0xFF);
    }
}

// ---------------------------------------------------------
void i2cList (void)
{
    byte val0;
    byte val1;

    Serial.println (F ("i2cList:"));

    for (int n = 0; n < 8; n++)  {
        val0 = i2cRead (n, IODIRA);
        val1 = i2cRead (n, IODIRB);

        printf (" %s: %d  0x%02x 0x%02x\n", __func__, n, val0, val1);
    }
}

// ---------------------------------------------------------
void i2cScan ()
{
    printf ("Scanning...\n");

    for (byte address = 1; address < 127; address++ ) {
        Wire.beginTransmission (address);
        byte error = Wire.endTransmission ();

        if (error == 0) {
            printf ("  I2C device found at address 0x%02x\n", address);
            nChip++;
        }
        else if (error==4)
            printf ("Unknow error at address 0x%02x\n", address);
    }

    if (nChip == 0)
        printf ("No I2C devices found\n");
    else
        printf ("done\n");
}

// ---------------------------------------------------------
// sequentially set each port bit
void
test (
    int     chip)
{
    Serial.print (F (" test: Gpio-A, chip 0x"));
    Serial.println (chip, HEX);

    for (byte bit = 0; bit < 8; bit++)  {
        i2cWrite (chip, GPIOA, 1 << bit);
        delay (500);
    }

    Serial.print (F (" test: Gpio-B, chip 0x"));
    Serial.println (chip, HEX);

    for (byte bit = 0; bit < 8; bit++)  {
        i2cWrite (chip, GPIOB, 1 << bit);
        delay (500);        // msec
    }
}

// ---------------------------------------------------------
int   tglTestCnt   = 0;
byte  tglTestEn    = 0;
unsigned long  tglTestPeriod = 250;

void
tglTest (void)
{
    static unsigned long msecLst;
    if (! tglTestEn || (msec - msecLst) < tglTestPeriod)
        return;
    msecLst  = msec;

    for (int chip = 0; chip < nChip; chip++)  {
        byte val = i2cRead (chip, GPIOA);
        if (0 == val)
            val = 0x80;
        else
            val >>= 1;
        i2cWrite (chip, GPIOA, val);
    }
}

// ---------------------------------------------------------
void bitTgl (
    byte     chip,
    byte     reg,
    byte     bit )
{
    byte    val0  = i2cRead (chip, reg);
    byte    val1 = val0 ^ (1 << bit);

    if (dbg) {
        sprintf (s, " %s: chip %d, bit %d, val0 0x%02x, val1 0x%02x",
            __func__, chip, bit, val0, val1);
        Serial.println (s);
    }

    i2cWrite (chip, reg, val1);
}

// ---------------------------------------------------------
void
i2cInit (void)
{
    Wire.begin ();    //start I2C bus

    i2cScan ();
    i2cCfg ();
    i2cList ();
}
