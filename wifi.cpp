# ifdef ESP32
#  include <WiFi.h>
# elif defined (ESP8266)
#  include <ESP8266WiFi.h>
# endif

// #include "ee.h"
#include "node.h"
// #include "pcRead.h"
// #include "signals.h"
#include "wifi_.h"

// -----------------------------------------------------------------------------
// WiFi and JMRI Server Definitions

const char *name  = "Node2";        // for now
const char *node  = "192.168.0.41";
const int   port  = 4445;           // 4986
int nNet;

char *ap0 = (char*)"wally";
char *ap1;

const char *sigMsg = "we have liftoff";

WiFiServer server (port);

// ---------------------------------------------------------
// connect to wifi
void wifiInit (
    const char *ssid,
    const char *pass )
{
    printf ("%s: ssid %s, password %s\n", __func__, ssid, pass);

    WiFi.mode (WIFI_STA);

    if (name [0])  {
#ifdef ESP32
        WiFi.hostname (name);
#else
        printf ("default hostname: %s\n", WiFi.hostname ().c_str ());
        WiFi.hostname (name);
        printf ("new hostname: %s\n", WiFi.hostname ().c_str ());
#endif
    }

    WiFi.begin (ssid, pass);
    server.begin ();
}

// -------------------------------------
// report if connected to wifi access point
bool
wifiCheck (void)
{
    static unsigned long msecLst = 0;

    if ( (msec - msecLst) < 1000)
        return false;
    msecLst = msec;

    printf (" %s:", __func__);

    if (WL_CONNECTED != WiFi.status ())  {
        printf (" not connected\n");
        return false;
   }

   IPAddress ip = WiFi.localIP ();

   printf (" connected %d:%d:%d:%d\n", ip [0], ip[1], ip [2], ip[3]);

   return true;
}

// ---------------------------------------------------------
#define MaxClient    10
WiFiClient clients [MaxClient];
IPAddress  ips     [MaxClient];
int nClient;

char buf [80];
int  bufIdx;

// ---------------------------------------------------------
// respond to a new connection
// https://docs.arduino.cc/library-examples/wifi-library/WiFiWebServer
void wifiRespond (
    int             nClient,
    unsigned long   msec)
{
   WiFiClient client = clients [nClient];

   bool currentLineIsBlank = true;
   while (client.connected ()) {
      if (client.available ()) {
        char c = client.read ();
 //     Serial.write (c);

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println ("HTTP/1.1 200 OK");
          client.println ("Content-Type: text/html");
 //       client.println ("Connection: close");  // the connection will be closed after completion of the response
 //       client.println ("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println ();

          client.println ("<!DOCTYPE HTML>");
          client.println ("<html>");
          client.print ("<h4>Hello World</h4>");
          client.println (msec);
          client.println ("</html>");
        }

        if (c == '\n')              // starting a new line
          currentLineIsBlank = true;
        else if (c != '\r')         // not \n norr
          currentLineIsBlank = false;
      }

#if 0
      delay (1);
      client.stop ();                   // close the connection:
#endif
    }
}

// ---------------------------------------------------------
// recognize a new connection
void wifiMonitor (void)
{
    // scan for new connections
    clients [nClient] = server.available ();

    if (! clients [nClient])
        return;

    ips [nClient] = clients [nClient].remoteIP ();
    for (int n = 0; n < nClient; n++)  {
        if (ips [n] == ips [n])
            return;
    }
        
    printf ("%s: new connection %d:%d:%d:%d\n", __func__,
        ips [nClient][0], ips [nClient][1], ips [nClient][2], ips [nClient][3]);

//      wifiRespond (nClient);
    nClient++;
}

// ---------------------------------------------------------
// common routine for sending strings to wifi and flushing
void
wifiUpdate (
    const char *msg )
{
    static unsigned long msecLst;

    if (msec - msecLst < 2000)
        return;
    msecLst = msec;

    for (int n = 0; n < nClient; n++)  {
        printf ("%s: %2d - %d:%d:%d:%d\n",
            __func__, n, ips [n][0], ips [n][1], ips [n][2], ips [n][3]);

        if (clients [n].connect (ips [n], 4445))  {
            delay (250);
            if (! clients [n].print (msg));
                printf ("    %s: %d failed to connect\n", __func__, n);
        }
        else
            printf ("%s: %d not connected\n", __func__, n);
    }
}

// -----------------------------------------------------------------------------
enum { ST_GET_CREDENTIALS, ST_INIT, ST_CHK, ST_MONITOR, ST_ERROR };

const char *wifiStStr [] = {
    "ST_GET_CREDENTIALS",
    "ST_INIT",
    "ST_CHK",
    "ST_MONITOR",
    "ST_ERROR"
};

int state = ST_INIT;
int stateLst;

// -------------------------------------
#define NUL   '\0'
void
wifiGetCredentials (void)
{
    if (NUL == ap0 [0] && NUL == ap1 [0])  {
        printf ("  %s: no ap0 or ap1\n", __func__);
        state = ST_ERROR;
        error = ERR_NO_SSID;
    }
}

// -------------------------------------
bool
wifiMatchSSID (
    char *ap )
{
    char *ssid = strtok (ap,   ",");
    char *pass = strtok (NULL, ",");

    printf ("%s: ssid %s, pass %s\n", __func__, ssid, pass);

    for (int i = 0; i < nNet; i++)  {
        if (! strcmp (ssid, WiFi.SSID (i).c_str ()))  {
            printf (" %s: match\n", __func__);
            wifiInit (ssid, pass);
            return true;
        }
    }

    return false;
}

// -------------------------------------
void
wifiReset (void)
{
    state = ST_GET_CREDENTIALS;
}

// -------------------------------------
// https://openlabpro.com/guide/scanning-of-wifi-on-esp32-controller/
void
wifiScan (void)
{
    printf ("%s:", __func__);

    // WiFi.scanNetworks will return the number of networks found
    nNet = WiFi.scanNetworks ();
    if (nNet == 0) {
        printf ("  no networks found\n");
    }
    else {
        printf ("  %2d networks found\n", nNet); 
        for (int i = 0; i < nNet; ++i) {
            printf (" %s: %2d - %s (%d) %s\n", __func__, i,
                WiFi.SSID (i).c_str (),
                WiFi.RSSI (i),
                WiFi.encryptionType (i) == WIFI_AUTH_OPEN ? "open" : "locked");
        }
    }
}

// -------------------------------------
void
wifi (void)
{
    if (stateLst != state)
        printf ("%s: %d %s\n", __func__, state, wifiStStr [state]);
    stateLst = state;

    switch (state)  {
    case ST_GET_CREDENTIALS:
        wifiGetCredentials ();

        if (ST_ERROR != state)  {
            printf ("%s: credentials acquired\n", __func__);

            wifiScan ();
            if (! wifiMatchSSID (ap0))  {
                error = ERR_NO_NET;
                state = ST_ERROR;
            }
            else  {
                state = ST_CHK;
            }
        }
        break;

    case ST_INIT:
        wifiInit ("wally", "Pan0rama");
        state = ST_CHK;
        break;

    case ST_CHK:
        if (wifiCheck ())
            state = ST_MONITOR;
        break;

    case ST_MONITOR:
        wifiMonitor ();
        wifiUpdate ("MD & WV");
        break;

    case ST_ERROR:
    default:
        break;
    }
}
