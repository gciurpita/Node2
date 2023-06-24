#ifndef SIGNALS_H
# define SIGNALS_H

# include "Arduino.h"

enum TwrSym {TwrNul, CB, HY, GA, TN, LT, TwrLast };

struct Twr {
    TwrSym      id;
    const char *sym;
    const char *name;
};

extern TwrSym    twr;
extern Twr       twrs [];
extern const int Ntwr;


// -------------------------------------
enum { P_Rd, P_Am, P_Gn, P_Wh, P_Size };

struct SigPin {
    byte        LedPins [P_Size];
    const char *label;
};

// -------------------------------------
struct SigMap {
    byte        blk;
    byte        blkNxt;

    byte        PinBlk;
    byte        idx;
    byte        On;

    TwrSym      twr;
    const char *desc;

    byte        occ;
    byte        occLst;
    byte        state;
    byte        stateLst;

    byte       *pOccNxt;
};

enum { LO = LOW, HI = HIGH };

// -------------------------------------
extern SigPin sigPin [];
extern const int NsigPin;

extern SigMap sigMap [];
extern const int NsigMap;

// -------------------------------------
void sigInit   (void);
void sigCheck  (void);
void sigDisp   (void);
void sigMsg    (const char *msg);

#endif
