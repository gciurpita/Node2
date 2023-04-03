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
    { {    5,    6,    7, 0xFF }, },
    { {    2,    3,    4, 0xFF }, },
    { {    0, 0xFF,    1, 0xFF }, },
};
const int NsigPin = sizeof(sigPin) / sizeof (SigPin);

// -------------------------------------
SigMap sigMap [] = {
//       __,_____._______________,_______________________,________   east
// 100 /     110       120                   130            140
//      ___,_____,_______________,_______________________,________/  west
//           111       121                   131            141
//      cumerberland     hyndman  sand patch   myersdale   garrett

 //   blk  nxt  pin  idx   on,  twr  desc
    { 100, 111,  33,   1,  LO,   CB, "B&O CB Loop" },
    { 111, 121,   0,   0,  LO,   CB, "B&O CB West" },

    { 121, 131,   0,   0,  HI,   HY, "B&O HY West" },
    { 131, 141,   8,   1,  LO,   HY, "B&O HY Sand" },

    { 141, 150,   0,   0,  LO,   OH, "B&O OH TunW" },
    { 150, 140,   0,   0,  LO,   OH, "B&O OH Loop" },
    { 140, 130,   0,   0,  LO,   OH, "B&O OH TunE" },

    { 130, 120,   0,   0,  LO,   OH, "B&O HY East" },
    { 120, 110,   9,   2,  LO,   HY, "B&O Sand Patch East" },

    { 110, 100,   0,   0,  LO,   CB, "B&O CB East" },

//       __,_____________________,__________,____________,________   east
// 200 /            210              220                    230
//      ___,_____________________,__________,____________,________/  west
//                  211              221                   231
//      cumerberland                  deal       myersdale   connelsville

    { 221,   0,  10,   3,  HI,   HY, "WM  Deal East" },
};
const int NsigMap = sizeof(sigMap) / sizeof (SigMap);

