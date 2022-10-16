const char *version   = "Koala WiFi - 221016d";
const char *name      = "Ohiopyle";

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

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
void loop ()
{
    wifiReceive ();

    if (Serial.available ())  {
        char c = Serial.read ();

        switch (c)  {
        case 'a':
            wifiSend ("antidisestablishmentarianism");
            break;

        case 'n':
            wifiSend (name);
            break;

        case 'o':
            wifiSend ("ok");
            break;
        }
    }
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

    wifiConnect ();
    jmriConnect ();
}
