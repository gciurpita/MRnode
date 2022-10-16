// model RR node

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>

// -----------------------------------------------------------------------------
const char* version = "Node 221012a";

// Set your access point network credentials
const char* ssid     = "wally";
const char* password = "Pan0rama";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

WiFiClient client;

unsigned debug = 0;

// -----------------------------------------------------------------------------
String readTemp() {
  return String("<!DOCTYPE html>  hello World");
}

// -----------------------------------------------------------------------------
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  Serial.println();
  Serial.println(version);
  
  Serial.print("Setting AP (Access Point)…");

  WiFi.begin ((char*)ssid, (char*)password);
  while (WiFi.status () != WL_CONNECTED)  {
    Serial.println (" connecting to WiFi");
    delay (1000);
  }

  IPAddress IP = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

#if 0
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemp().c_str());
  });
 
  // Start server
  server.begin();
#endif

  // -------------------------------------
  char host [] = "192.168.0.41";
  int  port    = 4445;

  do {
    printf (" connecting to %s:%d\n", host, port);
  } while (client.connect (host, port));
}
 
// -----------------------------------------------------------------------------
void loop()
{
    pcRead ();
}

// -----------------------------------------------------------------------------
// process single character commands from the PC
#define MAX_CHAR  10
char s [MAX_CHAR] = {};

int  analogPin = 0;

void
pcRead (void)
{

    static int  val = 0;

    if (Serial.available()) {
        int c = Serial.read ();

        switch (c)  {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            val = c - '0' + (10 * val);
            break;

        case 'A':
            analogPin = val;
            Serial.print   ("analogPin = ");
            Serial.println (val);
            val = 0;
            break;

        case 'D':
            debug ^= 1;
            break;

        case 'I':
            pinMode (val, INPUT);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" INPUT");
            val = 0;
            break;

        case 'O':
            pinMode (val, OUTPUT);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" OUTPUT");
            val = 0;
            break;

        case 'P':
            pinMode (val, INPUT_PULLUP);
            Serial.print   ("pinMode ");
            Serial.print   (val);
            Serial.println (" INPUT_PULLUP");
            val = 0;
            break;


        case 'a':
            Serial.print   ("analogRead: ");
            Serial.println (analogRead (val));
            val = 0;
            break;

        case 'c':
            digitalWrite (val, LOW);
            Serial.print   ("digitalWrite: LOW  ");
            Serial.println (val);
            val = 0;
            break;

        case 'h':
            printf ("%s: send hello\n", __func__);
            client.println ("Hello");
            client.flush   ();
            break;

        case 'p':
#if !defined(ARDUINO_ARCH_ESP32)
            analogWrite (analogPin, val);
            Serial.print   ("analogWrite: pin ");
            Serial.print   (analogPin);
            Serial.print   (", ");
            Serial.println (val);
            val = 0;
#endif
            break;

        case 'r':
            Serial.print   ("digitalRead: pin ");
            Serial.print   (val);
            Serial.print   (", ");
            Serial.println (digitalRead (val));
            val = 0;
            break;

        case 's':
            digitalWrite (val, HIGH);
            Serial.print   ("digitalWrite: HIGH ");
            Serial.println (val);
            val = 0;
            break;

        case 't':
            Serial.print   ("pinToggle ");
            Serial.println (val);
            digitalWrite (val, ! digitalRead (val));
            val = 0;
            break;

        case 'v':
            Serial.print ("\nversion: ");
            Serial.println (version);
            break;

        case '\n':          // ignore
            break;

        case '?':
            Serial.println ("\npcRead:\n");
            Serial.println ("    [0-9] append to #");
            Serial.println ("    A # - set analog pin #");
            Serial.println ("    D # - set debug to #");
            Serial.println ("    I # - set pin # to INPUT");
            Serial.println ("    O # - set pin # to OUTPUT");
            Serial.println ("    P # - set pin # to INPUT_PULLUP");
            Serial.println ("    a # - analogRead (pin #)");
            Serial.println ("    c # - digitalWrite (pin #, LOW)");
            Serial.println ("    p # -- analogWrite (analogPin, #)");
            Serial.println ("    r # - digitalRead (pin #)");
            Serial.println ("    s   - digitalWrite (pin #, HIGH)");
            Serial.println ("    t   -- toggle pin # output");
            Serial.println ("    v   - print version");
            Serial.println ("    \"   - read string");
            Serial.println ("    ?   - list of commands");
            break;

        default:
            Serial.print ("unknown char ");
            Serial.println (c,HEX);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
