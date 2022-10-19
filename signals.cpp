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
    { 110,  33, 12, 13, 14, 15, LOW  },
};
const int Nsig = sizeof(sig) / sizeof (Sig);

enum { Off = LOW, On = HIGH };

// -----------------------------------------------------------------------------
void sigUpdate (void)
{
    Sig *p = sig;
    for (unsigned n = 0; n < Nsig; n++, p++)  {
#if 0
        if (HIGH == digitalRead (p->PreOccPin))
            p->state = BlackOut;
        else
             if (LOW == digitalRead (p->BlkOccPin))
            p->state = Stop;
        else if (LOW == digitalRead (p->NxtOccPin))
            p->state = Approach;
        else
            p->state = Clear;

#else
        if (LOW == digitalRead (p->BlkOccPin))
            p->state = Stop;
        else
            p->state = Clear;
#endif

        if (p->stateLst != p->state)  {
            p->stateLst = p->state;

            sprintf (s, " blk %3d, %s", p->blockId, StateStr [p->state]);
            Serial.println (s);

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
    Sig *p = sig;
    for (unsigned n = 0; n < Nsig; n++, p++)  {
        pinMode (p->RedPin,   OUTPUT);
        digitalWrite (p->RedPin, On);
        delay (500);
        digitalWrite (p->RedPin, Off);

        if (p->AmberPin)  {
            pinMode (p->AmberPin, OUTPUT);
            digitalWrite (p->AmberPin, On);
            delay (500);
            digitalWrite (p->AmberPin, Off);
        }

        pinMode (p->GreenPin, OUTPUT);
        digitalWrite (p->GreenPin, On);
        delay (500);
        digitalWrite (p->GreenPin, Off);

        pinMode (p->WhitePin, OUTPUT);
        digitalWrite (p->WhitePin, On);
        delay (500);
        digitalWrite (p->WhitePin, Off);


        pinMode (p->BlkOccPin, INPUT_PULLUP);
    }
}
