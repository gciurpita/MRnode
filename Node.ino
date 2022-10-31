const char *version   = "Node WiFi - 221031b";
const char *name      = "Garrett";

#include "eeprom.h"
#include "signals.h"
#include "wifi.h"

unsigned long msec;

char s  [80];

// -----------------------------------------------------------------------------
enum { LedRed, LedGreen, LedBlue };
const byte PinsLed1 [] = { 16, 17, 18 };    // red, green, blue

enum { LedOff = HIGH, LedOn = LOW };

int  error;
int  errorState;
unsigned long msecLst;
unsigned long msecPeriod;

const unsigned long ErrPeriodOnOff = 500;
const unsigned long ErrPeriodPause = 2500;
const unsigned long ErrPeriodNoErr = 1000;

// -------------------------------------
void errorInit (void) {
    for (unsigned n = 0; n < sizeof(PinsLed1); n++)  {
        digitalWrite (PinsLed1 [n], LedOff);
        pinMode      (PinsLed1 [n], OUTPUT);
    }
}

// -------------------------------------
void errorMon ()
{
    if ( (msec - msecLst) >= msecPeriod)  {
        msecLst    = msec;

        if (! error)  {     // toggle green LED
            digitalWrite ( PinsLed1 [LedRed], LedOff);
            digitalWrite ( PinsLed1 [LedGreen],
                            !digitalRead ( PinsLed1 [LedGreen]));

            msecPeriod = ErrPeriodNoErr;
            errorState = 0;
        }
        else  {             // flash red LED
            if (--errorState == 0)
                msecPeriod = ErrPeriodPause;
            else if (errorState < 0)  {
                errorState = 2*error;
                msecPeriod = ErrPeriodOnOff;
            }

            digitalWrite ( PinsLed1 [LedGreen], LedOff);
            digitalWrite ( PinsLed1 [LedRed], ! (errorState % 2));
        }
    }
}

// -----------------------------------------------------------------------------
void pcRead ()
{
    static int  val = 0;
    static bool quote = false;
    static char str [40];
    static int  idx = 0;

    if (Serial.available ())  {
        char c = Serial.read ();

        if (quote)  {
            if ('"' == c)  {
                quote = false;
                str [idx] = '\0';
            }
            else  {
                str [idx++] = c;
                if (idx == sizeof(str)-1)  {
                    str [idx] = '\0';
                    quote     = false;
                }
            }
            return;
        }

        switch (c)  {
        case '"':
            quote = true;
            idx   = 0;
            break;

        case '0' ... '9':
            val = 10*val + c - '0';
            return;

        case 'a':
            wifiSend ("antidisestablishmentarianism");
            break;

        case 'c':
            printf (" pin %d  set\n", val);
            digitalWrite (val, LOW);
            break;

        case 'C':
            eepromClear ();
            break;

        case 'i':
            sigInit ();
            break;

        case 'I':
            pinMode (val, INPUT_PULLUP);
            break;

        case 'n':
            wifiSend (name);
            break;

        case 'o':
            wifiSend ("ok");
            break;

        case 'O':
            pinMode (val, OUTPUT);
            break;

        case 's':
            printf (" pin %d  set\n", val);
            digitalWrite (val, HIGH);
            break;

        case 'S':
            eepromScan ();
            break;

        case 'r':
            printf (" pin %d  %d\n", val, digitalRead (val));
            val = 0;
            break;

        case 'R':
            char buf [40];
            eepromRead (val, buf, 40);
            Serial.println (buf);
            val = 0;
            break;

        case 'W':
            eepromWrite (val, str);
            val = 0;
            break;

        case 'Y':
            wifiReset ();
            break;

        case '?':
            printf ("   a   send \"antidisestablishmentarianism\"\n");
            printf ("  #c   digitalWrite (#, LOW)\n");
            printf ("   C   eepromClear\n");
            printf ("   i   sigInit\n");
            printf ("  #I   pinMode (#, INPUT_PULLUP)\n");
            printf ("   n   send name\n");
            printf ("   o   send \"ok\"\n");
            printf ("  #O   pinMode (#, OUTPUT)\n");
            printf ("  #r   digitalRead (#)\n");
            printf ("  #R   eepromRead\n");
            printf ("  #s   digitalWrite (#, HIGH)\n");
            printf ("   S   eepromScan\n");
            printf ("   W   eepromtWrite\n");
            printf ("   ?   help\n");
            break;
        }

        val = 0;
    }
}

// -----------------------------------------------------------------------------
void loop ()
{
    msec = millis ();

#if 1
    wifiMonitor ();
#endif

    sigUpdate ();
    pcRead ();

    errorMon ();
}

// -----------------------------------------------------------------------------
void
setup (void)
{
    Serial.begin (9600);

    while (! Serial)
        ;

    Serial.println (version);

#if 1
    eepromInit ();
#endif
    sigInit ();
    errorInit ();
}
