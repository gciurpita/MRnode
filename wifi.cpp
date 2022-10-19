
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "node.h"
#include "wifi.h"

// -----------------------------------------------------------------------------
// WiFi and JMRI Server Definitions
WiFiClient          wifi;

const char *ssid  = "wally";
const char *pass  = "Pan0rama";


const char *host  = "192.168.0.41";
int         port  = 4445;

unsigned int debug = 0;

// ---------------------------------------------------------
// connect to wifi
void wifiConnect (void)
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
void nodeConnect (void)
{
    do {
        printf (" ... Node connecting - %s %d\n", host, port);

    } while (! wifi.connect(host, port));
    printf (" connected Node %s %d\n", host, port);

#if 0
    sprintf (s, "N%s", "greg ciurpita");
    wifiSend (s);
#endif
}

// ---------------------------------------------------------
// display wifi responses on serial monitor
void wifiReceive (void)
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

