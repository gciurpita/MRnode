const char *version   = "Koala WiFi - 221018a";
const char *name      = "Ohiopyle";

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <EEPROM.h>

// -----------------------------------------------------------------------------
// WiFi and JMRI Server Definitions
WiFiClient          wifi;

const char *ssid  = "wally";
const char *pass  = "Pan0rama";


const char *host  = "192.168.0.41";
int         port  = 4445;

unsigned int debug = 0;

#define S_SIZE 40
char s0 [S_SIZE];
char s1 [S_SIZE];
char s2 [S_SIZE];
char s3 [S_SIZE];
char s  [S_SIZE];

// ---------------------------------------------------------
// display up to 4 lines of text
enum { CLR };
void dispOled (
    const char  *s0,
    const char  *s1,
    const char  *s2,
    const char  *s3,
    bool         clr )
{
    if (s0)  Serial.println (s0);
    if (s1)  Serial.println (s1);
    if (s2)  Serial.println (s2);
    if (s3)  Serial.println (s3);
}

// ---------------------------------------------------------
// connect to wifi
static void wifiConnect (void)
{
    WiFi.mode (WIFI_STA);

    Serial.printf ("default hostname: %s\n", WiFi.hostname().c_str());
    WiFi.hostname (name);
    Serial.printf ("new hostname: %s\n", WiFi.hostname().c_str());

    WiFi.begin (ssid, pass);

    while (WL_CONNECTED != WiFi.status ())  {
        printf (" ... WiFi connecting - %s %s\n", ssid, pass);
        delay (1000);
   }

   IPAddress ip = WiFi.localIP ();

   Serial.print (" connected ");
   Serial.println (ip);
}

// -------------------------------------------------------------------
// connect to jmri
static void jmriConnect (void)
{
    do {
        sprintf (s0, "%d", port);
        printf (" ... JMRI connecting - %s %d\n", host, port);

    } while (! wifi.connect(host, port));
    printf (" connected JMRI %s %d\n", host, port);

#if 0
    sprintf (s, "N%s", "greg ciurpita");
    wifiSend (s);
#endif
}

// ---------------------------------------------------------
// display wifi responses on serial monitor
static void wifiReceive (void)
{
    static char cLst = 0;

    while (wifi.available()) {
        char c = wifi.read ();
        if ('\r' == c)
            continue;

        if ('\n' == cLst && '\n' == c)
            continue;

            Serial.write (c);
        cLst = c;
    }
}

// ---------------------------------------------------------
// common routine for sending strings to wifi and flushing
int dbgWifi = 1;        // can be set in simulation
void
wifiSend (
    const char*  s )
{
    if (dbgWifi)  {
        Serial.print ("wifiSend: ");
        Serial.println (s);
    }

#if 0
    int n = wifi.print (s);
 // wifi.flush ();
    printf ("%s: %d\n", __func__, n);
#else
    while (! wifi.print (s))  {
        printf (" ... re-connecting - %s %d\n", host, port);
        wifi.connect (host, port);
    }
#endif
}

// -----------------------------------------------------------------------------
#define EEPROM_SIZE  100

enum {
    ID_END      = 0,

    ID_HOSTNAME = 10,
    ID_SSID     = 11,
    ID_PASSWORD = 12,
    ID_MASTER   = 13,

    ID_IGNORE   = 0xFF,
    ID_NOT_FOUND = -1
};

struct TLV {
    byte        id;
    const char *lable;
}
tlvs [] = {
    { ID_HOSTNAME, "hostname" },
};

// -------------------------------------
int
eepromAddr (
    byte   type)
{
    for (int addr = 0; addr < EEPROM_SIZE; )  {
        byte id = EEPROM.read (addr);
        if (id == ID_END || id == type)
            return addr;

        addr += EEPROM.read (++addr)+1;
    }
    return ID_NOT_FOUND;
}

// -------------------------------------
void
eepromClear (void)
{
    sprintf (s, "%s:", __func__);
    Serial.print (s);

    for (unsigned n = 0; n < EEPROM_SIZE; n++)
        EEPROM.write (n, 0);
}
 
// -------------------------------------
void
eepromRead (
    byte   type,
    char  *buf,
    int    bufSize )
{
    sprintf (s, "%s:", __func__);
    Serial.println (s);

    int addr = eepromAddr (type);

    if (ID_NOT_FOUND == addr)  {
        Serial.println (" eepromRead - NOT_FOUND");
        return;
    }

    int len = EEPROM.read (++addr);

    if (len > (bufSize - 1))
        len = bufSize -1;

    for (int n = 0; n < len; n++)
        buf [n] = EEPROM.read (++addr);
    buf [bufSize-1] = '\0';
}

// -------------------------------------
void
eepromScan (void)
{
    sprintf (s, "%s:", __func__);
    Serial.print (s);

    for (unsigned n = 0; n < 32; n++)  {
        if (! (n % 8))
            Serial.println ();
        byte val = EEPROM.read (n);
        sprintf (s, " %02x", val);
        Serial.print (s);
    }
    Serial.println ();
}
 
// -------------------------------------
void
eepromWrite (
    byte        type,
    const char *text )
{
    sprintf (s, "%s: %s", __func__, text);
    Serial.println (s);

    int addr = eepromAddr (ID_END);
    EEPROM.write (addr++, type);

    byte len = strlen (text) + 1;
    EEPROM.write (addr++, len);

    for (unsigned n = 0; n < len; n++)
        EEPROM.write (addr++, text [n]);

    EEPROM.write (addr++, 0);
    EEPROM.commit ();
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
#ifdef WiFi
    wifiReceive ();
#endif

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
    jmriConnect ();
#endif

    EEPROM.begin (EEPROM_SIZE);
}
