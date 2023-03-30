// process commands from serial monitor

#include <Arduino.h>

#include "i2c.h"
#include "node.h"
#include "pcRead.h"

// -------------------------------------
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
            if (1 < debug)
                Serial.println (val);
            return;

        case ' ':
            break;

        case 'D':
            debug = val;
            break;

        case 'C':
            chip = val;
            break;

        case 'c':
            i2cWriteBit (val, 0);
            break;

        case 'l':
            i2cDump (0);
            break;

        case 'p':
            port = val;
            break;

        case 'r':
            printf (" %s: adr %d, %d\n", __func__, val, i2cReadBit (val));
            break;

        case 's':
            i2cWriteBit (val, 1);
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
            break;

        case 'V':
            Serial.print ("\nversion: ");
            Serial.println (version);
           break;

        case 'v':
            i2cList ();
            break;

        case 'X':
            i2cCfg ();
            break;

        case '?':
            printf ("  # D  debug\n");
            printf ("  # C  set chip 0-7 val\n");
            printf ("  # c  i2cwriteBit 0\n");
            printf ("    l  read all registers of chip\n");
            printf ("  # p  set port (0-output/1-input) val\n");
            printf ("  # r  i2xReadBit\n");
            printf ("  # s  i2cwriteBit 1\n");
            printf ("    T  en/disable tglTest\n");
            printf ("    t  sequentially set each bit in GPIO-A/B\n");
            printf ("  # w  write 8-bit val to chip/port\n");
            printf ("    V  version\n");
            printf ("    v  list chip registers\n");
            printf ("    X  reconfig chips\n");
            break;

        case '\n':      // process simulated button input
            break;

        default:
            Serial.print ("unknown: ");
            Serial.println (c);
            break;
        }
        val  = 0;
    }
}

