
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "eeprom.h"
#include "node.h"
#include "wifi.h"

// -----------------------------------------------------------------------------
// WiFi and JMRI Server Definitions
WiFiClient          wifi;

#if 0
const char *ssid  = "wally";
const char *pass  = "Pan0rama";

const char *host  = "192.168.0.41";
#else
const unsigned STR_SIZE = 40;

char host [STR_SIZE];
char ssid [STR_SIZE];
char pass [STR_SIZE];
#endif

int         port  = 4445;

unsigned int debug = 0;

// ---------------------------------------------------------
// connect to wifi
void wifiInit (void)
{
    WiFi.mode (WIFI_STA);

#ifdef ESP32
    WiFi.hostname (name);
#else
    printf ("default hostname: %s\n", WiFi.hostname().c_str());
    WiFi.hostname (name);
    printf ("new hostname: %s\n", WiFi.hostname().c_str());
#endif

    WiFi.begin (ssid, pass);
}

// -------------------------------------
bool
wifiCheck (void)
{
    if (WL_CONNECTED != WiFi.status ())  {
        return false;
   }

   IPAddress ip = WiFi.localIP ();

   Serial.print (" connected ");
   Serial.println (ip);

   return true;
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
        printf ("wifiSend: %s\n", s);
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

// ---------------------------------------------------------
enum { ST_GET_CREDENTIALS, ST_AP, ST_MASTER, ST_NODES, ST_ERROR };

const char *wifiStStr [] = {
    "ST_GET_CREDENTIALS",
    "ST_AP",
    "ST_MASTER",
    "ST_NODES",
    "ST_ERROR"
};

int state = ST_GET_CREDENTIALS;
int stateLst;

// -------------------------------------
void
wifiGetCredentials (void)
{
    if (! eepromRead (ID_HOSTNAME, host, sizeof(host)))  {
        printf ("%s: no hostname\n", __func__);
        state = ST_ERROR;
        error = ERR_NO_HOST;
    }

    if (! eepromRead (ID_SSID, ssid, sizeof(host)))  {
        printf ("%s: no ssid\n", __func__);
        state = ST_ERROR;
        error = ERR_NO_SSID;
    }

    if (! eepromRead (ID_PASSWORD, pass, sizeof(host))) {
        printf ("%s: no password\n", __func__);
        state = ST_ERROR;
        error = ERR_NO_PASS;
    }
}

// -------------------------------------
void
wifiReset (void)
{
    state = ST_GET_CREDENTIALS;
}

// -------------------------------------
void
wifiMonitor (void)
{
    if (stateLst != state)
        printf ("%s: %d %s\n", __func__, state, wifiStStr [state]);
    stateLst = state;

    switch (state)  {
    case ST_GET_CREDENTIALS:
        wifiGetCredentials ();

        if (ST_ERROR != state)  {
            printf ("%s: credentials acquired\n", __func__);
            wifiInit ();
            state = ST_AP;
        }
        break;

    case ST_AP:
        if (wifiCheck ())
            state = ST_MASTER;
        break;

    case ST_MASTER:
        break;

    case ST_ERROR:
    default:
        break;
    }
}
