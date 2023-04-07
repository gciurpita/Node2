const char *version = "MD & WV Node2 - 230407a";

#include <Arduino.h>

#include "i2c.h"
#include "node.h"
#include "pcRead.h"
#include "signals.h"
#include "wifi_.h"

unsigned debug   = 1;
int   error   = 0;
int   ledMode = 0;
char  s [100];

unsigned long msec;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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

    sigCheck ();
//  wifi ();
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

#ifdef I2C
    i2cInit ();
#endif
    sigInit ();

#ifdef SIM
    Serial.println (" --------- SIM ---------");
    for (unsigned n = 0; n < N_XS; n++)  {
        pinMode (pinXs [n], INPUT_PULLUP);
        stateXs [n] = digitalRead (pinXs [n]);
    }
#endif
}
