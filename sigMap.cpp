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
    { {    4,   __,   __,   __ }, },    // brewery
};
const int NsigPin = sizeof(sigPin) / sizeof (SigPin);

// -------------------------------------
SigMap sigMap [] = {
//       __,_____._______________,_______________________,________      east
// 100 /     110       120                   130
//      ___,_____,_______________,_______________________,________/ 141 west
//           111       121                   131
//      cumerberland     hyndman  sand patch   myersdale   garrett

    { 141, 140,   33,     0,  HI,   TN, "Exp GA" },

 //   blk  nxt   pin  idx+1 on,  twr  desc
    { 100, 111,    8,     2,  LO,   CB, "B&O CB Loop" },
    { 111, 121,    9,     0,  HI,   CB, "B&O CB West" },

    { 121, 131,   12,     0,  HI,   HY, "B&O HY West" },
    { 131, 141,    8,     1,  HI,   HY, "B&O HY Sand" },

    { 141, 130,   10,     2,  LO,   GA, "B&O GA Loop" },
    { 130, 120,    9,     0,  LO,   GA, "B&O GA West" },

    { 120, 110,    9,     3,  HI,   HY, "B&O Sand Patch East" },

    { 110, 100,   10,     5,  HI,   CB, "B&O CB East" },

// short circuits
    { 123,   0,   12,    26,  HI,   HY, "B&O HY West" },
    { 133,   0,    8,    27,  HI,   HY, "B&O HY Sand" },
    { 122,   0,    9,    27,  HI,   HY, "B&O Sand Patch East" },

// display
    { 100, 111,   __,   100,  HI,   DP, "B&O CB Loop" },
    { 111, 121,   __,   101,  HI,   DP, "B&O CB West" },
    { 121, 131,   __,   103,  HI,   DP, "B&O HY West" },
    { 131, 141,   __,   113,  HI,   DP, "B&O HY Sand" },

    { 141, 130,   __,   112,  HI,   DP, "B&O DP Loop" },
    { 130, 120,   __,   114,  HI,   DP, "B&O DP West" },
    { 120, 110,   __,   115,  HI,   DP, "B&O Sand Patch East" },
    { 110, 100,   __,   102,  HI,   DP, "B&O CB East" },

// red leds
    { 102,   0,   __,   116,  HI,   DP, "B&O CB Loop" },
    { 113,   0,   __,   117,  HI,   DP, "B&O CB West" },
    { 123,   0,   __,   119,  HI,   DP, "B&O HY West" },
    { 133,   0,   __,   129,  HI,   DP, "B&O HY Sand" },
    { 143,   0,   __,   128,  HI,   DP, "B&O DP Loop" },
    { 132,   0,   __,   130,  HI,   DP, "B&O DP West" },
    { 122,   0,   __,   131,  HI,   DP, "B&O Sand Patch East" },
    { 112,   0,   __,   118,  HI,   DP, "B&O CB East" },

//       __,_____________________,__________,____________,_____      east
// 200 /            210              220         230
//      ___,_____________________,__________,____________,_____/ 240 west
//                  211              221         231
//      cumerberland                  deal       myersdale   connelsville

    { 200,   0,  11,     4,  HI,   CB, "WM  CB Loop" },
    { 210,   0,  24,     0,  HI,   CB, "WM  CB East" },
    { 211,   0,  25,     0,  HI,   CB, "WM  CB West" },

    { 220,   0,  11,     0,  HI,   HY, "WM  Deal East" },
    { 221,   0,  10,     4,  HI,   HY, "WM  Deal West" },

    { 230,   0,  11,     0,  HI,   GA, "WM  GA East" },
    { 231,   0,  12,     0,  HI,   GA, "WM  GA West" },
    { 240,   0,  13,     0,  HI,   GA, "WM  GA Loop" },

// short circuits
    { 222,   0,  11,    23,  HI,   HY, "WM  Deal East" },
    { 223,   0,  10,    24,  HI,   HY, "WM  Deal West" },

// display
    { 200, 211,   __,   104,  HI,   DP, "WM CB Loop" },
    { 211, 221,   __,   105,  HI,   DP, "WM CB West" },
    { 221, 231,   __,   108,  HI,   DP, "WM Deal West" },

    { 231, 240,   __,   110,  HI,   DP, "WM GA West" },
    { 240, 230,   __,   111,  HI,   DP, "WM GA Loop" },

    { 230, 220,   __,   109,  HI,   DP, "WM GA East" },
    { 220, 210,   __,   107,  HI,   DP, "WM Deal East" },
    { 210, 200,   __,   106,  HI,   DP, "WM CB East" },

// red leds
    { 202,   0,   __,   120,  HI,   DP, "WM CB Loop" },
    { 213,   0,   __,   121,  HI,   DP, "WM CB West" },
    { 223,   0,   __,   124,  HI,   DP, "WM Deal West" },
    { 233,   0,   __,   126,  HI,   DP, "WM GA Sand" },
    { 242,   0,   __,   127,  HI,   DP, "WM GA Loop" },

    { 232,   0,   __,   125,  HI,   DP, "WM GA West" },
    { 222,   0,   __,   123,  HI,   DP, "WM Deal East" },
    { 212,   0,   __,   122,  HI,   DP, "WM CB East" },

};
const int NsigMap = sizeof(sigMap) / sizeof (SigMap);

