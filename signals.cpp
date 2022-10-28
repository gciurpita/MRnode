// railroad signal(s) driver

#include <Arduino.h>

#include "node.h"
#include "signals.h"

// -----------------------------------------------------------------------------
enum { BlackOut, Stop, Approach, Clear };

const char *StateStr [] = {
    "BlackOut",
    "Stop",
    "Approach",
    "Clear"
};

struct Sig {
    byte    blockId;
    byte    BlkOccPin;

    byte    RedPin;
    byte    AmberPin;
    byte    GreenPin;
    byte    WhitePin;

    byte    On;

    byte    BlkNxt;
    byte    BlkPre;

    byte    state;
    byte    stateLst;
};

Sig sig [] = {
 //    ID  Occ Red Yel Grn Whi
    { 100,  32,  0,  2,  4,  5, HIGH },
    { 110,  33, 12, 13, 14, 15, LOW, 32  },
};
const int Nsig = sizeof(sig) / sizeof (Sig);

enum { Off = LOW, On = HIGH };

// -----------------------------------------------------------------------------
void sigDisp (void)
{
    Sig *p = sig;
    for (unsigned n = 0; n < Nsig; n++, p++)  {
        printf (" %2d  %2d %2d %2d %2d\n",
            p->BlkOccPin, p->RedPin, p->AmberPin, p->GreenPin, p->WhitePin);
    }
}

// -------------------------------------
void sigUpdate (void)
{
 // printf ("%s:\n", __func__);

    Sig *p = sig;
    for (unsigned n = 0; n < Nsig; n++, p++)  {
#if 0
        if (HIGH == digitalRead (p->PreOccPin))
            p->state = BlackOut;
        else
#endif

        if (LOW == digitalRead (p->BlkOccPin))
            p->state = Stop;
        else if (0 != p->BlkNxt && LOW == digitalRead (p->BlkNxt))
            p->state = Approach;
        else
            p->state = Clear;

        if (p->stateLst != p->state)  {
            p->stateLst = p->state;
            delay (50);

            printf (" %s: blk %d, state %d %s\n",
                __func__, p->blockId, p->state, StateStr [p->state]);

            digitalWrite (p->GreenPin, ! p->On);
            digitalWrite (p->RedPin,   ! p->On);
            digitalWrite (p->WhitePin, ! p->On);

            if (p->AmberPin)
                digitalWrite (p->AmberPin, ! p->On);

            switch (p->state) {
            case Stop:
                digitalWrite (p->RedPin,   p->On);
                break;
            case Approach:
                digitalWrite (p->AmberPin, p->On);
                break;
            case Clear:
                digitalWrite (p->GreenPin, p->On);
                digitalWrite (p->WhitePin, p->On);
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
void sigInit (void)
{
    printf ("%s:\n", __func__);

    Sig *p = sig;
    for (unsigned n = 0; n < Nsig; n++, p++)  {
        printf (" %s: blk %d\n", __func__, p->blockId);

        pinMode (p->RedPin,   OUTPUT);
        digitalWrite (p->RedPin, p->On);
        delay (500);
        digitalWrite (p->RedPin, ! p->On);

        if (p->AmberPin)  {
            pinMode (p->AmberPin, OUTPUT);
            digitalWrite (p->AmberPin, p->On);
            delay (500);
            digitalWrite (p->AmberPin, ! p->On);
        }

        pinMode (p->GreenPin, OUTPUT);
        digitalWrite (p->GreenPin, p->On);
        delay (500);
        digitalWrite (p->GreenPin, ! p->On);

        pinMode (p->WhitePin, OUTPUT);
        digitalWrite (p->WhitePin, p->On);
        delay (500);
        digitalWrite (p->WhitePin, ! p->On);


        pinMode (p->BlkOccPin, INPUT_PULLUP);
    }
}
