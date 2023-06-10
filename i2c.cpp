#include <Arduino.h>
#include "Wire.h"  // I2C operations

#include "i2c.h"
#include "node.h"
#include "mpc23017.h"

//   MCP23017
//   8 - GPB0 1   28 GPA7 - 7
//   9 - GPB1 2   27 GPA6 - 6
//  10 - GPB2 3   26 GPA5 - 5
//  11 - GPB3 4   25 GPA4 - 4
//  12 - GPB4 5   24 GPA3 - 3
//  13 - GPB5 6   23 GPA2 - 2
//  14 - GPB6 7   22 GPA1 - 1
//  15 - GPB7 8   21 GPA0 - 0

//  Vdd 9   20 INTA
//  Vss 10  19 INTB
//   NC 11  18 Nreset
//  SCL 12  17 A2
//  SDA 13  16 A1
//   NC 14  15 A0

// -------------------------------------
//      MPC    Uno  Mega  Esp32
//  SCK  12 <-  A5   21    22
//  SDA  13 <-  A4   20    21

// -----------------------------------------------------------------------------
const char * portNames [] = {
    "IODIRA",
    "IODIRB",
    "IPOLA",
    "IPOLB",

    "GPINTENA",
    "GPINTENB",
    "DEFVALA",
    "DEFVALB",

    "INTCONA",
    "INTCONB",
    "IOCON",
    "IOCON",

    "GPPUA",    // 0x0c
    "GPPUB",
    "INTFA",
    "INTFB",

    "INTCAPA",
    "INTCAPB",
    "GPIOA",    // 0x12
    "GPIOB",

    "OLATA",
    "OLATB",
};
const int Nport = sizeof(portNames) / sizeof(char*);

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
    byte    chip,
    byte    port,
    byte    val )
{
    chip += chip < Chip20 ? Chip20 : 0;

    if (8 & debug)
        printf ("   %s: c 0x%02x, p %2d, v 0x%02x\n",
            __func__, chip, port, val);

    Wire.beginTransmission (chip);
    Wire.write (port);    
    Wire.write (val);    
    Wire.endTransmission ();
}

// ---------------------------------------------------------
void
i2cWritePortBit (
    byte    adr,
    byte    port,
    bool    b )
{
    if (0xFF == adr)
        return;

    byte bit  = 1 << (adr & 7);
    byte chip = adr >> 4;
    port     += adr & 0x8 ? 1 : 0;

    byte val = i2cRead (chip, port);

    if (4 & debug)
        printf ("  %s: adr %2d, %d, c %d, p %2d, b 0x%02x, val 0x%02x",
            __func__, adr, b, chip, port, bit, val);

    if (b)
        val  |=  bit;
    else
        val  &= ~bit;

    if (1 < debug)
        printf (" - 0x%02x\n", val);

    i2cWrite (chip, port, val);
}

// ---------------------------------------------------------
void
i2cWriteBit (
    byte    adr,
    bool    b )
{
    i2cWritePortBit (adr, GPIOA, b);
}

// ---------------------------------------------------------
byte
i2cRead (
    byte    chip,
    byte    port )
{
    if (8 & debug)
        return 0xFF;

    chip += chip < Chip20 ? Chip20 : 0;

    Wire.beginTransmission (chip);
    Wire.write (port);    
    Wire.endTransmission ();

    Wire.requestFrom ((int)chip, 1); //get 1 byte
    byte val =  Wire.read ();

    if (16 & debug)  {
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
bool
i2cReadBit (
    byte    adr )
{
    byte bit  = 1 << (adr & 7);
    byte chip = adr >> 4;

    byte port = adr & 0x8 ? GPIOB : GPIOA;
    return bit & i2cRead (chip, port);
}

// ---------------------------------------------------------
bool
i2cReadPortBit (
    byte    adr,
    byte    port )
{
    byte bit  = 1 << (adr & 7);
    byte chip = adr >> 4;
    port     += adr & 0x8 ? 1 : 0;
    return bit & i2cRead (chip, port);
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
void
i2cDump (
    byte chip)
{
    for (int port = 0; port < Nport; port++)
        printf (" %s: chip %d, port %2d 0x%02x %s\n", __func__,
            chip, port, i2cRead (chip, port), portNames [port]);
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
        printf (" No I2C devices found\n");
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

    if (4 & debug) {
        sprintf (s, " %s: chip %d, bit %d, val0 0x%02x, val1 0x%02x",
            __func__, chip, bit, val0, val1);
        Serial.println (s);
    }

    i2cWrite (chip, reg, val1);
}

// ---------------------------------------------------------
const byte PinReset = 19;
void i2cReset (void)
{
    pinMode (PinReset, OUTPUT);
    digitalWrite (PinReset, LOW);
    delay (500);
    digitalWrite (PinReset, HIGH);
}

// ---------------------------------------------------------
void
i2cInit (void)
{
    Wire.begin ();    //start I2C bus

    i2cReset ();

    i2cScan ();
 // i2cCfg ();
 // i2cList ();
 // i2cDump (0);
}

// ---------------------------------------------------------
void
i2cMon (void)
{
}
