// railroad signal(s) driver

#include <Arduino.h>

#include "i2c.h"
#include "node.h"
// #include "ee.h"
#include "signals.h"
#include "wifi_.h"

// -----------------------------------------------------------------------------
enum { BlackOut, Stop, Approach, Clear };

TwrSym  twr = HY;

const char *StateStr [] = {
    "BlackOut",
    "Stop",
    "Approach",
    "Clear"
};

enum { Off = LOW, On = HIGH };

// -----------------------------------------------------------------------------
void sigDisp (void)
{
    printf ("%s:\n", __func__);

    printf ("    %3s %3s %3s %3s", "Blk", "Nxt", "pin", "idx");
    printf ("  %3s %3s %3s  %3s %3s %3s %3s",
        "blk", "nxt", "pre", "Red", "Amb", "Grn", "Whi");
    printf ("  %2s %3s %3s %s\n", "On", "twr", "Sym", "desc");

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        printf ("    %3d %3d %2d %d", s->blk, s->blkNxt, s->PinBlk, s->idx);

#if 0
        SigPin *p = & sigPin [s->idx];
        printf ("  %3d %3d %3d ", p->BlkOcc, p->BlkNxt, p->BlkPre);

        for (unsigned i = 0; i < P_Size; i++)
            printf (" %3d", p->LedPins [i]);
#endif

        printf ("  %2d %3d %3s %s", s->On, s->twr, twrs [s->twr].sym, s->desc);
        printf ("\n");
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
// get block status from table
bool
sigBlkGet (
    byte blk )
{
 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->blk == blk)
            return s->occ;
    }
    printf ("Error: %s - blk not found, %d\n", __func__, blk);
    return false;
}

// -------------------------------------
// scan blocks on node
void sigBlkMon (void)
{
 // printf ("%s:\n", __func__);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr)
            continue;

        byte occ = ! digitalRead (s->PinBlk);   // active LOW
        if (s->occ != occ)  {
            s->occ = occ;
            printf ("%s: blk %d, pin %d, occ %d\n",
                    __func__, s->blk, s->PinBlk, s->occ);
            sigReport (s);
        }
    }
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
    char *msg)
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

        if (sigBlkGet (s->blk))
            s->state = Stop;
        else if (sigBlkGet (s->blkNxt))
            s->state = Approach;
        else
            s->state = Clear;

        if (s->stateLst != s->state)  {
            s->stateLst = s->state;
            delay (50);

            printf (" %s: twr %d %s, blk %d, state %d %s,  %s\n", __func__,
                twr, twrs [twr].sym ,
                s->blk, s->state, StateStr [s->state], s->desc);

            // turn all LEDs off
            SigPin *p = & sigPin [s->idx];

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
    printf ("%s: twr %d %s\n", __func__, twr, twrs [twr].sym);

    SigMap *s = sigMap;
    for (int n = 0; n < NsigMap; n++, s++)  {
        if  (s->twr != twr)
            continue;

        printf (" %s: blk %d, nxt %d, pin %2d, idx %d\n",
            __func__, s->blk, s->blkNxt, s->PinBlk, s->idx);
#ifdef I2C
# if 0
        byte pin = s->PinBlk;
        byte bit  = 1 << (pin & 7);
        byte chip = pin >> 4;

        byte port = pin & 0x4 ? IODIRB : IODIRA;
        byte val  =  bit & i2cRead (chip, port);
        i2cWrite (chip, port, val);

        port = pin & 0x4 ? GPPUB : GPPUA;
        val  =  bit & i2cRead (chip, port);
        i2cWrite (chip, port, val);
# else
        i2cWritePortBit (s->PinBlk, IODIRA, Inp);
        i2cWritePortBit (s->PinBlk, GPPUA,  1);
# endif
#else
        pinMode (p->BlkOcc, INPUT_PULLUP);
#endif

        if (s->idx)  {
            SigPin *p = & sigPin [s->idx - 1];
            for (unsigned i = 0; i < P_Size; i++)  {
                printf (" %s: %d ledPin %d\n", __func__, i, p->LedPins [i]);
                if (0xFF != p->LedPins [i])  {
#ifdef I2C
# if 0
                    byte pin = p->LedPins [i];
                    printf (" %2d", pin);
                    bit  = 1 << (pin & 7);
                    chip = pin >> 4;
                    port = pin & 0x4 ? IODIRB : IODIRA;
                    val  = ~bit & i2cRead (chip, port);
                    i2cWrite (chip, port, val);
# else
                    i2cWritePortBit (p->LedPins [i], IODIRA, Out);
# endif
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
        printf ("\n");
    }
}
