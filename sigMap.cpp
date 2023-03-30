#include "signals.h"

//   MCP23017
// GPB7 1   28 GPA7
// GPB6 2   27 GPA6
// GPB5 3   26 GPA5
// GPB4 4   25 GPA4
// GPB3 5   24 GPA3
// GPB2 6   23 GPA2
// GPB1 7   22 GPA1
// GPB0 8   21 GPA0

//  Vdd 9   20 INTA
//  Vss 10  19 INTB
//   NC 11  18 Nreset
//  SCL 12  17 A2
//  SDA 13  16 A1
//   NC 14  15 A0

#define P(chp, port, pin)    (chp<<4 | port<<3 | pin)

// -------------------------------------
Twr twrs [] = {
    { TwrNul, 0, 0, 0 },

#undef CLUB
#if CLUB
    { CB, "CB", "Cumberland", "192.168.0.11" },
    { HY, "HY", "Hyndman",    "192.168.0.22" },
    { OH, "OH", "Ohiopyle",   "192.168.0.33" },
#else
    { CB, "CB", "Cumberland", "192.168.0.100" },
    { HY, "HY", "Hyndman",    "192.168.0.244" },
    { OH, "OH", "Ohiopyle",   "192.168.0.33" },
    { LT, "LT", "Laptop",     "192.168.0.41" },
#endif
};
const int Ntwr = sizeof(twrs)/sizeof(Twr);

// -------------------------------------
SigPin sigPin [] = {
 //      Red   Yel   Grn   Whi
    { {    5,    6,    7, 0xFF }, "Hyndman" },
    { {    2,    3,    4, 0xFF }, "Sand patch" },
    { {    0, 0xFF,    1, 0xFF }, "Deal" },
};
const int NsigPin = sizeof(sigPin) / sizeof (SigPin);

// -------------------------------------

SigMap sigMap [] = {
 //   blk  nxt  pin  idx   on,  twr  desc
    { 100, 111,  33,   1,  LO,   CB, "B&O CB Loop" },
    { 110, 100,  32,   2,  LO,   CB, "B&O VI East" },
    { 111, 121,   0,   0,  LO,   CB, "B&O CB West" },

    { 120, 110,  15,   1,  LO,   HY, "B&O HY East" },
 // { 121, 131,  13,   0,  LO,   HY, "B&O HY West" },
 // { 131, 141,  32,   3,  LO,   HY, "B&O SP West" },
    { 221, 231,  13,   3,  HI,   HY, "B&O HY West" },

    { 130, 120,  33,   1,  LO,   OH, "B&O MY East" },
    { 141, 130,  32,   2,  LO,   OH, "B&O OH Loop" },
};
const int NsigMap = sizeof(sigMap) / sizeof (SigMap);

