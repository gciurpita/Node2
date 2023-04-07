#include "signals.h"

//   MCP23017
//   8 - GPB0  1  28 GPA7 - 7
//   9 - GPB1  2  27 GPA6 - 6
//  10 - GPB2  3  26 GPA5 - 5
//  11 - GPB3  4  25 GPA4 - 4
//  12 - GPB4  5  24 GPA3 - 3
//  13 - GPB5  6  23 GPA2 - 2
//  14 - GPB6  7  22 GPA1 - 1
//  15 - GPB7  8  21 GPA0 - 0

//        Vdd  9  20 INTA
//        Vss 10  19 INTB
//         NC 11  18 Nreset
//        SCL 12  17 A2
//        SDA 13  16 A1
//         NC 14  15 A0

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
    { {    7,    6,    5,    4 }, },
    { {    3,    2,    1,    0 }, },
    { {   14, 0xFF,   15, 0xFF }, },
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
    { 100, 111,  33,   1,  HI,   CB, "B&O CB Loop" },
    { 111, 121,   0,   0,  HI,   CB, "B&O CB West" },

    { 121, 131,   0,   0,  HI,   HY, "B&O HY West" },
    { 131, 141,   8,   1,  HI,   HY, "B&O HY Sand" },

    { 141, 150,   0,   0,  HI,   OH, "B&O OH TunW" },
    { 150, 140,   0,   0,  HI,   OH, "B&O OH Loop" },
    { 140, 130,   0,   0,  HI,   OH, "B&O OH TunE" },

    { 130, 120,   0,   0,  HI,   OH, "B&O HY East" },
    { 120, 110,   9,   2,  HI,   HY, "B&O Sand Patch East" },

    { 110, 100,   0,   0,  HI,   CB, "B&O CB East" },

//       __,_____________________,__________,____________,________   east
// 200 /            210              220                    230
//      ___,_____________________,__________,____________,________/  west
//                  211              221                   231
//      cumerberland                  deal       myersdale   connelsville

    { 221,   0,  10,   3,  LO,   HY, "WM  Deal East" },
};
const int NsigMap = sizeof(sigMap) / sizeof (SigMap);

