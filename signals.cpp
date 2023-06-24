// railroad signal(s) driver

#include <Arduino.h>

#include "i2c.h"
#include "node.h"
// #include "ee.h"
#include "signals.h"
#include "wifi.h"

// -----------------------------------------------------------------------------
enum { BlackOut, Stop, Approach, Clear };

TwrSym  twr;

const char *StateStr [] = {
    "BlackOut",
    "Stop",
    "Approach",
    "Clear"
};

enum { Off = LOW, On = HIGH };

// -----------------------------------------------------------------------------
void
sigDisp (void)
{
    printf ("%s:\n", __func__);

    printf ("    %3s %3s %3s %3s", "Blk", "Nxt", "pin", "idx");
    printf (" %2s %3s", "On", "twr");
    printf (" %3s %3s %3s %3s", "RD", "YE", "GN", "WH");
    printf (" %3s  %s\n", "occ", "desc");

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr)
            continue;

        printf ("    %3d %3d %3d %3d", s->blk, s->blkNxt, s->PinBlk, s->idx);
        printf (" %2s %3s", s->On ? "HI" : "LO", twrs [s->twr].sym);

        if (0 == s->idx)
            printf ("   -   -   -   -");
        else {
            SigPin *p = & sigPin [s->idx-1];
            for (unsigned i = 0; i < P_Size; i++)
                printf (" %3d", p->LedPins [i]);
        }

        printf (" %3s", s->occ ? "Occ" : "___");
        printf ("  %s\n", s->desc);
    }
}

// -------------------------------------
// send occupancy changes to other nodes
void sigReport (
    SigMap  *q )
{
    char msg [20];
    sprintf (msg, "blk %d %d,", q->blk, q->occ);
    printf (" %s: %s\n", __func__,  msg);

#if 0
    nodeSend (LT, msg);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if (s->blkNxt == q->blk && s->twr != twr)
            nodeSend (s->twr, msg);
    }
#endif
}

// -------------------------------------
// return blk idx from table
static SigMap *
_sigGetBlk (
    byte blk )
{
 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if (s->blk == blk)
            return s;
    }
    printf ("Error: %s - blk not found, %d\n", __func__, blk);
    return NULL;
}

// -------------------------------------
// get block status from table
#if 0
static byte *
_sigBlkPocc (
    byte blk )
{
 // printf ("%s:\n", __func__);

    if (0 == blk)
        return 0;

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->blk == blk)
            return & s->occ;
    }
    printf ("Error: %s - blk not found, %d\n", __func__, blk);
    return 0;
}
#endif

// -------------------------------------
// scan blocks on node
void sigBlkMon (void)
{
#if 0
    static unsigned long msecLst;
    if (msec - msecLst < 1500)
        return;

    msecLst = msec;
#endif

 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr)
            continue;

#ifdef I2C
        byte occ = ! i2cReadBit (s->PinBlk);   // active LOW
#else
        byte occ = ! digitalRead (s->PinBlk);   // active LOW
#endif
 //     printf ("  %s: pin %2d, occ %d\n", __func__, s->PinBlk, occ);

        if (s->occ != occ)  {
            s->occ = occ;
            printf ("%s: blk %d, pin %d, occ %d\n",
                    __func__, s->blk, s->PinBlk, s->occ);
            sigReport (s);
        }
        else if (2 & debug)  {
            printf ("%s: blk %d, pin %d, occ %d\n",
                    __func__, s->blk, s->PinBlk, s->occ);
        }
    }
    if (2 & debug)
        delay (1000);
}

// -------------------------------------
// set block status
void
sigBlkSet (
    byte blk,
    byte occ )
{
 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->blk == blk)  {
            s->occ = occ;
            return;
        }
    }
    printf ("Error: %s - blk not found, %d\n", __func__, blk);
    return;
}

// -------------------------------------
// process signal msgs
void sigMsg (
    const char *msg)
{
    int  blk;
    int  occ;

    printf ("%s: %s\n", __func__, msg);

    if (! strncmp (msg, "blk", 3))  {
        sscanf (msg, "blk %d %d", &blk, & occ);
        sigBlkSet (blk, occ);
    }
}

// -------------------------------------
// update signals on node
void sigUpdate (void)
{
 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr || 0 == s->idx)
            continue;

        // update state
        s->state = Clear;
        if (s->occ) {
            s->occLst = s->occ;
            s->state  = Stop;
        }
        else if (s->pOccNxt)  {
            if (*s->pOccNxt)
                s->state = Approach;
        }

        // update signal if state change
        if (s->stateLst != s->state)  {
            s->stateLst = s->state;
            delay (50);

            printf (" %s: twr %d %s, blk %d, state %d %s,  %s\n", __func__,
                twr, twrs [twr].sym ,
                s->blk, s->state, StateStr [s->state], s->desc);

            // turn all LEDs off
            SigPin *p = & sigPin [s->idx - 1];

#ifdef I2C
            for (unsigned i = 0; i < P_Size; i++)  {
                if (0xFF != p->LedPins [i])
                    i2cWriteBit  (p->LedPins [i], ! s->On);
            }

            // turn LEDs on
            switch (s->state) {
            case Stop:
                i2cWriteBit  (p->LedPins [P_Rd], s->On);
                break;
            case Approach:
                i2cWriteBit  (p->LedPins [P_Am], s->On);
                break;
            case Clear:
                i2cWriteBit  (p->LedPins [P_Gn], s->On);
                i2cWriteBit  (p->LedPins [P_Wh], s->On);
                break;
            }
#else
            for (unsigned i = 0; i < P_Size; i++)  {
                if (0 != p->LedPins [i])
                    digitalWrite (p->LedPins [i], ! s->On); 
            }

            // turn LEDs on
            switch (s->state) {
            case Stop:
                digitalWrite (p->LedPins [P_Rd], s->On);
                break;
            case Approach:
                digitalWrite (p->LedPins [P_Am], s->On);
                break;
            case Clear:
                digitalWrite (p->LedPins [P_Gn], s->On);
                digitalWrite (p->LedPins [P_Wh], s->On);
                break;
            }
#endif
        }
    }
}

// -------------------------------------
void sigCheck (void)
{
 // printf ("%s:\n", __func__);

    sigBlkMon ();
    sigUpdate ();
}

// -----------------------------------------------------------------------------
void sigInit (void)
{
    printf ("%s:\n", __func__);

    for (int n = 1; n < Ntwr; n++)  {
#if 1
        if (! strcmp (twrs [n].name, host))  {
            twr = (TwrSym) n;
            break;
        }
#elif 0
        printf ("  %s: %d: \n", __func__, n);
#else
        printf ("  %s: %d: %d %s %s\n", __func__,
            n, twrs [n].id, twrs [n].sym, twrs [n].name);
#endif
    }

    if (! twr)  {
        printf (" %s: unknown twr %s\n", __func__, host);
        return;
    }

    printf (" %s: twr %d %s\n", __func__, twr, twrs [twr].sym);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr)
            continue;

        // get occ ptr to next blk
        if (s->blkNxt)  {
            SigMap *t = _sigGetBlk (s->blkNxt);
            if (t)  {
                s->pOccNxt = & t->occ;
                printf ("   %s: blk %d, nxt %d, %s\n",
                    __func__, s->blk, s->blkNxt, t->desc);
            }
        }

        printf (" %s: blk %d, nxt %d, pin %2d, idx %d",
            __func__, s->blk, s->blkNxt, s->PinBlk, s->idx);

        if (s->idx)  {
            byte  *p = sigPin [s->idx - 1].LedPins;
            printf (", %2d %2d %2d %2d", p [0], p [1], p [2], p [3]);
        }
        printf ("  %s\n", s->desc);

#ifdef I2C
        i2cWritePortBit (s->PinBlk, IODIRA, Inp);
        i2cWritePortBit (s->PinBlk, GPPUA,  1);
#else
        pinMode (p->BlkOcc, INPUT_PULLUP);
#endif

        if (s->idx)  {
            SigPin *p = & sigPin [s->idx - 1];
            for (unsigned i = 0; i < P_Size; i++)  {
                printf ("  %s: %d ledPin %d\n", __func__, i, p->LedPins [i]);
                if (0xFF != p->LedPins [i])  {
#ifdef I2C
                    i2cWritePortBit (p->LedPins [i], IODIRA, Out);
#else
                    byte pin = p->LedPins [i];
                    printf (" %2d", pin);
                    pinMode (p->LedPins [i],   OUTPUT);
                    digitalWrite (p->LedPins [i], s->On);
                    delay (500);
                    digitalWrite (p->LedPins [i], ! s->On);
#endif
                }
            }
        }
    }
}
