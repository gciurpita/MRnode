const char *version   = "Node WiFi - 221028c";
const char *name      = "Garrett";

#include "eeprom.h"
#include "signals.h"
#include "wifi.h"

const byte PinsLed1 [] = { 16, 17, 18 };    // red, green, blue

char s  [80];

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

        case '?':
            Serial.println ("   a   send \"antidisestablishmentarianism\"");
            Serial.println ("  #c   digitalWrite (#, LOW)");
            Serial.println ("   C   eepromClear");
            Serial.println ("   i   sigInit");
            Serial.println ("  #I   pinMode (#, INPUT_PULLUP)");
            Serial.println ("   n   send name");
            Serial.println ("   o   send \"ok\"");
            Serial.println ("  #O   pinMode (#, OUTPUT)");
            Serial.println ("  #r   digitalRead (#)");
            Serial.println ("  #R   eepromRead");
            Serial.println ("  #s   digitalWrite (#, HIGH)");
            Serial.println ("   S   eepromScan");
            Serial.println ("   W   eepromtWrite");
            Serial.println ("   ?   help");
            break;
        }

        val = 0;
    }
}

// -----------------------------------------------------------------------------
void loop ()
{
#define WiFi
#ifdef WiFi
    wifiReceive ();
#endif

    sigUpdate ();
    pcRead ();
}

// -----------------------------------------------------------------------------
void
setup (void)
{
    Serial.begin (9600);

    for (unsigned n = 0; n < sizeof(PinsLed1); n++)  {
        digitalWrite (PinsLed1 [n], HIGH);
        pinMode      (PinsLed1 [n], OUTPUT);
    }

    while (! Serial)
        ;

    Serial.println (version);

#ifdef WiFi
    wifiConnect ();
    nodeConnect ();
#endif

#if 0
    eepromInit ();
#endif
    sigInit ();
}
