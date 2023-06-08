#include <Arduino.h>

#include "eeProm.h"
#include "node.h"
#include "pcRead.h"
#include "signals.h"
#include "wifi.h"

// -----------------------------------------------------------------------------
#define MAX_TOKS   10
static int    _nToks;
static char * _toks [MAX_TOKS];
static int    _vals [MAX_TOKS];

int
_tokenize (
    char *s)
{
    int n = 0;
    for (_toks [n] = strtok (s, " "); _toks [n]; ) {
        _vals [n] = atoi (_toks [n]);
     // printf ("   %2d: %6d %s\n", n, _vals [n], _toks [n]);
        _toks [++n] = strtok (NULL, " ");
    }

    return _nToks = n;
}

// -----------------------------------------------------------------------------
void
_cli (
    char * buf )
{
    int nTok = _tokenize (buf);

    if (! strcmp (_toks [0], "host") && 2 == nTok)
        strcpy (host, _toks [1]);

    else if (! strcmp (_toks [0], "pass") && 2 == nTok)
        strcpy (pass, _toks [1]);

    else if (! strcmp (_toks [0], "ssid") && 2 == nTok)
        strcpy (ssid, _toks [1]);

    else if (! strcmp (_toks [0], "ipAdd") && 2 == nTok)
        wifiIpAdd (_toks [1]);

    else if (! strcmp (_toks [0], "ipClr"))
        wifiIpClr ();

    else if (! strcmp (_toks [0], "ipList"))
        wifiIpList ();

    else {
        printf ("%s: invalid input\n", __func__);
        for (int n = 0; n < nTok; n++)
            printf (" %s: %2d  %s\n", __func__, n, _toks [n]);
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

        case '_':
            {
                char buf [80];
                int n = Serial.readBytesUntil (';', buf, sizeof(buf)-1);
                buf [n] = '\0';
                _cli (buf);
            }
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

        case 'l':
            eepromLoad ();
            break;

        case 'o':
            wifiSend ("ok");
            break;

        case 'p':
            printf (" processor: %s\n", processor);
            break;

        case 'O':
            pinMode (val, OUTPUT);
            break;

        case 'r':
            printf (" pin %d  %d\n", val, digitalRead (val));
            val = 0;
            break;

        case 'R':
            ESP.restart ();
            break;

        case 's':
            printf (" pin %d  set\n", val);
            digitalWrite (val, HIGH);
            break;

        case 'S':
            eepromScan ();
            break;

        case 't':
            printf (" twr: %d %s\n", twr, "twr-name");
            break;

        case 'v':
            printf (" version: %s\n", version);
            break;

        case 'U':
            eepromUpdate ();
            break;

        case 'W':
            eepromWrite (val, str);
            val = 0;
            break;

        case 'w':
            wifiReset ();
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
            printf ("   l   eepromtLoad\n");
            printf ("   n   send name\n");
            printf ("   o   send \"ok\"\n");
            printf ("   p   processor type\n");
            printf ("  #O   pinMode (#, OUTPUT)\n");
            printf ("  #r   digitalRead (#)\n");
            printf ("   R   reset\n");
            printf ("  #s   digitalWrite (#, HIGH)\n");
            printf ("   S   eepromScan\n");
            printf ("   t   twr\n");
            printf ("   u   eepromtUpdate\n");
            printf ("   v   version\n");
            printf ("   W   eepromtWrite\n");
            printf ("   W   wifiReset\n");
            printf ("   ?   help\n");
            break;
        }

        val = 0;
    }
}

