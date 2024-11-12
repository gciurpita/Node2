
#ifdef ESP32
# include <WiFi.h>
#elif defined(ESP8266)
# include <ESP8266WiFi.h>
#endif

#include "AsyncUDP.h"

#include "eeprom.h"
#include "node.h"
#include "signals.h"
#include "wifi.h"

int dbgWifi = 1;

static void _wifiScan (void);
static void _wifiMsg  (const char *msg);

// -----------------------------------------------------------------------------
// WiFiClient          wifi;
AsyncUDP udp;

char host [STR_SIZE] = "";
char ssid [STR_SIZE] = "";
char pass [STR_SIZE] = "";

static int  port  = 4445;

// ---------------------------------------------------------
enum { ST_NUL, ST_INIT, ST_CHK, ST_CFG_UDP, ST_UP, ST_ERROR };

const char *wifiStStr [] = {
    "ST_NUL",
    "ST_INIT",
    "ST_CHK",
    "ST_CFG_UDP",
    "ST_UP",
    "ST_ERROR"
};

int state    = ST_NUL;
int stateLst = ST_NUL;

// -------------------------------------
static bool
_wifiCheck (void)
{
    static int           fails = 0;
    static unsigned long msecLst = 0;

    if ( (msec - msecLst) < 1000)
        return false;
    msecLst = msec;

    printf (" %s:", __func__);

#if 0
    if (6 <= fails)  {
        _wifiScan ();
        return false;
    }
#endif

    if (WL_CONNECTED != WiFi.status ())  {
        printf (" not connected\n");
        fails++;
        return false;
   }

   IPAddress ip = WiFi.localIP ();

   printf (" connected %d:%d:%d:%d\n", ip [0], ip[1], ip [2], ip[3]);

   return true;
}

// -------------------------------------
// https://arduino.clanweb.eu/udp-control-esp32.php?lang=en

static void
_wifiCfgUdp (void)
{
    printf (" %s:\n", __func__);

    if (udp.listen (port)) {
        Serial.print ("  UDP Listening on IP: ");
        Serial.println (WiFi.localIP ());

        udp.onPacket ([] (AsyncUDPPacket packet) {
#if 0
            Serial.print   ("UDP Packet Type: ");
            Serial.println (packet.isBroadcast ()
                        ? "Broadcast"
                        : packet.isMulticast () ? "Multicast" : "Unicast" );
            Serial.print   (" From: ");
            Serial.print   (packet.remoteIP ());
            Serial.print   (":");
            Serial.println (packet.remotePort ());
            Serial.print   (" To: ");
            Serial.print   (packet.localIP ());
            Serial.print   (":");
            Serial.println (packet.localPort ());
            Serial.print   (" Length: ");
            Serial.println (packet.length ());
            Serial.print   (" Data: ");
            Serial.write   (packet.data (), packet.length ());
            Serial.println ();
#endif

            _wifiMsg ((const char *) packet.data ());
        });
  }
}

// -------------------------------------
// connect to wifi
void _wifiInit (void)
{
    printf (" %s:\n", __func__);

    _wifiScan ();
    WiFi.mode (WIFI_STA);

    WiFi.hostname (host);

    printf ("%s: ssid %s, pass %s\n", __func__, ssid, pass);
    WiFi.begin (ssid, pass);
}

// -------------------------------------
static void
_wifiMsg (
    const char *msg)
{
    if (dbgWifi)
        printf (" %s:\n", __func__);

    sigMsg (msg);
}

// -------------------------------------
void
wifiReset (void)
{
    printf ("%s: ssid %s, pass %s, host %s\n", __func__, ssid, pass, host);
    state = ST_NUL;
}

// -------------------------------------
// https://openlabpro.com/guide/scanning-of-wifi-on-esp32-controller/
static void
_wifiScan (void)
{
    printf ("%s:\n", __func__);

    // WiFi.scanNetworks will return the number of networks found
    WiFi.mode (WIFI_OFF);
    int nNet = WiFi.scanNetworks ();
    if (nNet == 0) {
        printf ("  %s: no networks found\n", __func__);
    }
    else {
        printf ("  %s: %2d networks found\n", __func__, nNet);
        if (0 > nNet)
            nNet = -nNet;
        for (int i = 0; i < nNet; ++i) {
            printf (" %s: %2d - %s (%d) %s\n", __func__, i,
                WiFi.SSID (i).c_str (),
                WiFi.RSSI (i),
                WiFi.encryptionType (i) == WIFI_AUTH_OPEN ? "open" : "locked");
        }
    }
}

// ---------------------------------------------------------
// common routine for sending strings to wifi and flushing
void
wifiSend (
    const char*  msg )
{
    if (ST_UP != state)
        return;

    if (dbgWifi)  {
        printf ("wifiSend: %s\n", msg);
    }

    udp.broadcast (msg);
}

// -------------------------------------
void
wifiMonitor (void)
{
    if (stateLst != state)
        printf ("%s: %d %s\n", __func__, state, wifiStStr [state]);
    stateLst = state;

    switch (state)  {
    case ST_NUL:
        if (ssid [0])
            state = ST_INIT;
        else {
            printf ("%s: no SSID\n", __func__);
            state = ST_ERROR;
        }
        break;

    case ST_INIT:
        _wifiInit ();
        state = ST_CHK;
        break;

    case ST_CHK:
        if (_wifiCheck ())
            state = ST_CFG_UDP;
        break;

    case ST_CFG_UDP:
        _wifiCfgUdp ();
        state = ST_UP;
        break;

    case ST_UP:
        break;

    case ST_ERROR:
    default:
        break;
    }
}
