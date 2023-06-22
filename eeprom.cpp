
#include <EEPROM.h>

#include "eeprom.h"
#include "wifi.h"

extern char *s;

// -----------------------------------------------------------------------------
#define EEPROM_SIZE  100

struct TLV {
    byte        id;
    const char *label;
    void       *location;
    void (*func) (char * s);
}
tlvs [] = {
    { ID_HOSTNAME, "hostname", host },
    { ID_SSID,     "ssid",     ssid },
    { ID_PASSWORD, "password", pass },
 // { ID_IP,       "ip",       0,   wifiIpAdd },
};
const int Ntlvs = sizeof(tlvs) / sizeof(TLV);

// -------------------------------------
int
eepromAddr (
    byte   type)
{
    for (int addr = 0; addr < EEPROM_SIZE; )  {
        byte id = EEPROM.read (addr);
 //     printf (" %s: 0x%02x\n", __func__, id);

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
    printf ("%s:\n", __func__);

    EEPROM.write (0, ID_END);
    EEPROM.commit ();
}
 
// -------------------------------------
void eepromLoad ();

void
eepromInit (void)
{
    EEPROM.begin (EEPROM_SIZE);
    eepromLoad ();

    printf ("%s: ssid %s, pass %s, host %s\n", __func__, ssid, pass, host);
}
 
// -------------------------------------
void
eepromLoad (void)
{
    int addr = 0;
    int type;
    int len;

    printf ("%s:\n", __func__);

    while (1) {
        type = EEPROM.read (addr++);
        if (ID_END == type || 0 == type)
            return;

        len   = EEPROM.read (addr++);

        char buf [80];
        for (int n = 0; n < len; n++)
             buf [n] = EEPROM.read (addr++);

        printf (" %s: type %02d, len %3d %s\n", __func__, type, len, buf);

        // -------------------------------------
        int  n;
        for (n = 0; n < Ntlvs; n++)  {
            if (tlvs [n].id == type)  {
                if (char *p = (char *) tlvs [n].location)
                    strcpy (p, buf);

                else if (tlvs [n].func)
                    (tlvs [n].func) (buf);

                else
                    printf ("  %s: no *location or *func\n", __func__);
                break;
            }
        }

        if (Ntlvs == n)  {
 //         addr += len;
            printf ("  %s: ID %d not found\n", __func__, type);
        }
    }
}
 
// -------------------------------------
bool
eepromRead (
    byte   type,
    char  *buf,
    int    bufSize )
{
    printf ("%s:\n", __func__);

    int addr = eepromAddr (type);

    if (ID_NOT_FOUND == addr)  {
        printf (" %s: %d NOT_FOUND\n", __func__, type);
        return false;
    }

    int len = EEPROM.read (++addr);

    if (len > (bufSize - 1))
        len = bufSize -1;

    for (int n = 0; n < len; n++)
        buf [n] = EEPROM.read (++addr);
    buf [bufSize-1] = '\0';

    printf (" %s: %d %s\n", __func__, type, buf);
    return true;
}

// -------------------------------------
void
eepromScan (void)
{
    printf ("%s:", __func__);

    for (unsigned n = 0; n < 32; n++)  {
        if (! (n % 8))
            printf ("\n    ");
        byte val = EEPROM.read (n);
        printf (" %02x", val);
    }
    printf ("\n");
}
 
// -------------------------------------
void eepromWrite ( byte type, const char *text);

void
eepromUpdate (void)
{
    EEPROM.write (0, ID_END);

    for (int n = 0; n < Ntlvs; n++)  {
        if (tlvs [n].location)
            eepromWrite (tlvs [n].id, (const char *)tlvs [n].location);

#if 0
        else if (ID_IP == tlvs [n].id)  {
            char *s;
            for (int idx = 0; NULL != (s = wifiIpGet (idx)); idx++)
                eepromWrite (tlvs [n].id, s);
        }
#endif
    }
}
 
// -------------------------------------
void
eepromWrite (
    byte        type,
    const char *text )
{
    printf ("%s: %d %s\n", __func__, type, text);

    int addr = eepromAddr (ID_END);
    if (ID_NOT_FOUND == addr)  {
        printf (" %s: not found - eeprom full\n", __func__);
        return;
    }

    printf (" %s: addr 0x%02x\n", __func__, addr);

    EEPROM.write (addr++, type);

    byte len = strlen (text) + 1;
    EEPROM.write (addr++, len);

    for (unsigned n = 0; n < len; n++)
        EEPROM.write (addr++, text [n]);

    EEPROM.write (addr++, ID_END);
    EEPROM.commit ();
}

