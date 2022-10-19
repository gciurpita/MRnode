const char *version   = "Node WiFi - 221019a";
const char *name      = "Ohiopyle";

#include "eeprom.h"
#include "signals.h"
#include "wifi.h"

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
            break;

        case 'a':
            wifiSend ("antidisestablishmentarianism");
            break;

        case 'C':
            eepromClear ();
            break;

        case 'n':
            wifiSend (name);
            break;

        case 'o':
            wifiSend ("ok");
            break;

        case 'S':
            eepromScan ();
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
            Serial.println ("   C   eepromClear");
            Serial.println ("  #R   eepromRead");
            Serial.println ("   S   eepromScan");
            Serial.println ("   W   eepromtWrite");
            Serial.println ("   ?   help");
            break;
        }
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
    Serial.begin (115200);
    delay (1000);

    while (! Serial)
        ;

    Serial.println (version);

#ifdef WiFi
    wifiConnect ();
    nodeConnect ();
#endif

    eepromInit ();
    sigInit ();
}
