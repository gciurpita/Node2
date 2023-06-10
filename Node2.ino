const char *version = "MD & WV Node2 - 230610b";    // from 0425a

#include <Arduino.h>

#include "eeprom.h"
#include "i2c.h"
#include "node.h"
#include "pcRead.h"
#include "signals.h"
#include "wifi.h"

unsigned debug   = 1;
int   error   = 0;
int   ledMode = 0;
char  s [100];

unsigned long msec;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const char *processor = "TTGO";
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
ledStatus ()
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
    ledStatus ();

 // wifiMonitor ();
    sigCheck ();
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

    eepromInit ();
    i2cInit ();
    sigInit ();

#ifdef SIM
    Serial.println (" --------- SIM ---------");
    for (unsigned n = 0; n < N_XS; n++)  {
        pinMode (pinXs [n], INPUT_PULLUP);
        stateXs [n] = digitalRead (pinXs [n]);
    }
#endif
}
