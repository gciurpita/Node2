// process commands from serial monitor

#include <Arduino.h>

#include "i2c.h"
#include "node.h"
#include "pcRead.h"

void
pcRead (void)
{
    static byte chip = 0;
    static byte port = 0;
    static int  val  = 0;

#define None    -1
    if (Serial.available ()) {
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
            if (1 < dbg)
                Serial.println (val);
            break;

        case ' ':
            val = 0;
            break;

        case 'D':
            dbg = val;
            val = 0;
            break;

        case 'c':
            chip = val;
            val  = 0;
            break;

        case 'p':
            port = val;
            val  = 0;
            break;

        case 'R':
            i2cCfg ();
            break;

        case 'r':
            Serial.print ("read: chip ");
            Serial.print (chip, HEX);
            Serial.print (", port ");
            Serial.print (port, HEX);
            Serial.print (",  ");
            Serial.println (i2cRead (chip, port));
            break;

        case 'S':
            for (int port = 0; port <= 0x15; port++)  {
                Serial.print (" read: chip ");
                Serial.print (chip, HEX);
                Serial.print (", port ");
                Serial.print (port, HEX);
                Serial.print (",  ");
                Serial.println (i2cRead (chip, port), HEX);
            }
            break;

        case 'T':
            tglTestEn  ^= 1;
            ledMode = tglTestEn ? 1 : 0; 
            break;

        case 't':
            test (chip);
            break;

        case 'w':
            i2cWrite (chip, port, val);
            val  = 0;
            break;

        case 'V':
            Serial.print ("\nversion: ");
            Serial.println (version);
           break;

        case 'v':
            i2cList ();
            break;

        case '?':
            printf ("  # D  debug\n");
            printf ("  # c  set chip 0-7 val\n");
            printf ("  # p  set port (0-output/1-input) val\n");
            printf ("    R  reconfig chips\n");
            printf ("    r  read chip, port\n");
            printf ("    S  read all registers of chip\n");
            printf ("    T  en/disable tglTest\n");
            printf ("    t  sequentially set each bit in GPIO-A/B\n");
            printf ("  # w  write 8-bit val to chip/port\n");
            printf ("    V  version\n");
            printf ("    v  list chip registers\n");
            break;

        case '\n':      // process simulated button input
            break;

        default:
            Serial.print ("unknown: ");
            Serial.println (c);
            break;
        }
    }
}

