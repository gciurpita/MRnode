
#include <EEPROM.h>

#include "eeprom.h"

extern char *s;

// -----------------------------------------------------------------------------
#define EEPROM_SIZE  100

struct TLV {
    byte        id;
    const char *lable;
}
tlvs [] = {
    { ID_HOSTNAME, "hostname" },
};

// -------------------------------------
int
eepromAddr (
    byte   type)
{
    for (int addr = 0; addr < EEPROM_SIZE; )  {
        byte id = EEPROM.read (addr);
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
    sprintf (s, "%s:", __func__);
    Serial.print (s);

    for (unsigned n = 0; n < EEPROM_SIZE; n++)
        EEPROM.write (n, 0);
}
 
// -------------------------------------
void
eepromInit (void)
{
    EEPROM.begin (EEPROM_SIZE);
}
 
// -------------------------------------
void
eepromRead (
    byte   type,
    char  *buf,
    int    bufSize )
{
    sprintf (s, "%s:", __func__);
    Serial.println (s);

    int addr = eepromAddr (type);

    if (ID_NOT_FOUND == addr)  {
        Serial.println (" eepromRead - NOT_FOUND");
        return;
    }

    int len = EEPROM.read (++addr);

    if (len > (bufSize - 1))
        len = bufSize -1;

    for (int n = 0; n < len; n++)
        buf [n] = EEPROM.read (++addr);
    buf [bufSize-1] = '\0';
}

// -------------------------------------
void
eepromScan (void)
{
    sprintf (s, "%s:", __func__);
    Serial.print (s);

    for (unsigned n = 0; n < 32; n++)  {
        if (! (n % 8))
            Serial.println ();
        byte val = EEPROM.read (n);
        sprintf (s, " %02x", val);
        Serial.print (s);
    }
    Serial.println ();
}
 
// -------------------------------------
void
eepromWrite (
    byte        type,
    const char *text )
{
    sprintf (s, "%s: %s", __func__, text);
    Serial.println (s);

    int addr = eepromAddr (ID_END);
    EEPROM.write (addr++, type);

    byte len = strlen (text) + 1;
    EEPROM.write (addr++, len);

    for (unsigned n = 0; n < len; n++)
        EEPROM.write (addr++, text [n]);

    EEPROM.write (addr++, 0);
    EEPROM.commit ();
}

