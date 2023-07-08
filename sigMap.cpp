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
    { TwrNul, 0, 0           },
    { CB, "CB", "Cumberland" },
    { HY, "HY", "Hyndman"    },
    { GA, "GA", "Garrett"    },
    { TN, "TN", "Test-Node"  },
    { DP, "DP", "Display"    },
};
const int Ntwr = sizeof(twrs)/sizeof(Twr);

// -------------------------------------
SigPin sigPin [] = {
 //      Red   Yel   Grn   Whi
    { {    7,    6,    5,    4 }, },
    { {    7,    6,    5,   __ }, },
    { {    3,    2,    1,    0 }, },
    { {   14,   __,   15,   __ }, },
};
const int NsigPin = sizeof(sigPin) / sizeof (SigPin);

// -------------------------------------
SigMap sigMap [] = {
//       __,_____._______________,_______________________,________      east
// 100 /     110       120                   130             140
//      ___,_____,_______________,_______________________,________/ 150 west
//           111       121                   131             141
//      cumerberland     hyndman  sand patch   myersdale   garrett

    { 141, 140,   33,     0,  HI,   TN, "Exp GA" },

 //   blk  nxt   pin  idx+1 on,  twr  desc
    { 100, 111,   33,     1,  HI,   CB, "B&O CB Loop" },
    { 111, 121,   __,     0,  HI,   CB, "B&O CB West" },

    { 121, 131,   __,     0,  HI,   HY, "B&O HY West" },
    { 131, 141,    8,     1,  HI,   HY, "B&O HY Sand" },

    { 141, 130,    8,     2,  LO,   GA, "B&O GA Loop" },
    { 130, 120,    9,     0,  LO,   GA, "B&O GA West" },

    { 120, 110,    9,     3,  HI,   HY, "B&O Sand Patch East" },

    { 110, 100,   __,     0,  HI,   CB, "B&O CB East" },

    { 100, 111,   __,   104,  HI,   DP, "B&O CB Loop" },
    { 111, 121,   __,   105,  HI,   DP, "B&O CB West" },
    { 121, 131,   __,   108,  HI,   DP, "B&O HY West" },
    { 131, 141,   __,   110,  HI,   DP, "B&O HY Sand" },

    { 141, 130,   __,   111,  HI,   DP, "B&O DP Loop" },
    { 130, 120,   __,   109,  HI,   DP, "B&O DP West" },
    { 120, 110,   __,   107,  HI,   DP, "B&O Sand Patch East" },
    { 110, 100,   __,   106,  HI,   DP, "B&O CB East" },

//       __,_____________________,__________,____________,________   east
// 200 /            210              220                    230
//      ___,_____________________,__________,____________,________/  west
//                  211              221                   231
//      cumerberland                  deal       myersdale   connelsville

    { 221,   0,  10,     4,  HI,   HY, "WM  Deal East" },
};
const int NsigMap = sizeof(sigMap) / sizeof (SigMap);

