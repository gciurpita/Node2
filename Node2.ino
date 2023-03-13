
const char version [] = "MD & WV Node2 - 230313a";

#include "Wire.h"  // I2C operations
#include "mpc23017.h"

int   dbg     = 1;
int   ledMode = 0;
char  s [100];

unsigned long msec;

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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// process commands from serial monitor
void
pcRead (void)
{
    static int  val  = 0;

#define None    -1
    if (Serial.available ()) {
        int c = Serial.read ();

        switch (c)  {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = c - '0' + (10 * val);
            if (1 < dbg)
                Serial.println (val);
            break;

        case ' ':
            val = 0;
            break;

        case 'D':
            dbg = val;
            val = 0;
            break;

        case 'c':
            chip = val;
            val  = 0;
            break;

        case 'p':
            port = val;
            val  = 0;
            break;

        case 'R':
            i2cCfg ();
            break;

        case 'r':
            Serial.print ("read: chip ");
            Serial.print (chip, HEX);
            Serial.print (", port ");
            Serial.print (port, HEX);
            Serial.print (",  ");
            Serial.println (i2cRead (chip, port));
            break;

        case 'S':
            for (int port = 0; port <= 0x15; port++)  {
                Serial.print (" read: chip ");
                Serial.print (chip, HEX);
                Serial.print (", port ");
                Serial.print (port, HEX);
                Serial.print (",  ");
                Serial.println (i2cRead (chip, port), HEX);
            }
            break;

        case 'T':
            tglTestEn  ^= 1;
            ledMode = tglTestEn ? 1 : 0; 
            break;

        case 't':
            test (chip);
            break;

        case 'w':
            i2cWrite (chip, port, val);
            val  = 0;
            break;

        case 'V':
            Serial.print ("\nversion: ");
            Serial.println (version);
           break;

        case 'v':
            i2cList ();
            break;

        case '?':
            printf ("  # D  debug\n");
            printf ("  # c  set chip 0-7 val\n");
            printf ("  # p  set port (0-output/1-input) val\n");
            printf ("    R  reconfig chips\n");
            printf ("    r  read chip, port\n");
            printf ("    S  read all registers of chip\n");
            printf ("    T  en/disable tglTest\n");
            printf ("    t  sequentially set each bit in GPIO-A/B\n");
            printf ("  # w  write 8-bit val to chip/port\n");
            printf ("    V  version\n");
            printf ("    v  list chip registers\n");
            break;

        case '\n':      // process simulated button input
            break;

        default:
            Serial.print ("unknown: ");
            Serial.println (c);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
const byte PinLedGrn = 5;

struct LedPeriod {
    unsigned long   period [2];
};
LedPeriod ledPeriod [] = {
    { 1900, 100 },
    {   50, 200 },
    {  100, 150 },
};

void
ledStrobe ()
{
    static unsigned long msecLst;
    int state = digitalRead (PinLedGrn);
    if (msec - msecLst >= ledPeriod [ledMode].period [state])  {
        msecLst = msec;
        digitalWrite (PinLedGrn, ! state);
    }
}

// -----------------------------------------------------------------------------
void
loop ()
{
    msec = millis ();
    ledStrobe ();

    pcRead ();

    tglTest ();
}

// -----------------------------------------------------------------------------
void setup ()
{
    Serial.begin (115200);
    delay (500);
    Serial.println (version);

    pinMode (PinLedGrn, OUTPUT);

    Wire.begin ();    //start I2C bus

    i2cScan ();
    i2cCfg ();
    i2cList ();

#ifdef SIM
    Serial.println (" --------- SIM ---------");
    for (unsigned n = 0; n < N_XS; n++)  {
        pinMode (pinXs [n], INPUT_PULLUP);
        stateXs [n] = digitalRead (pinXs [n]);
    }
#endif
}
