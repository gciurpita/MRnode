
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
    { ID_SSID,     "ssid" },
    { ID_PASSWORD, "password" },
    { ID_MASTER,   "master" },
};

// -------------------------------------
int
eepromAddr (
    byte   type)
{
    for (int addr = 0; addr < EEPROM_SIZE; )  {
        byte id = EEPROM.read (addr);
        printf (" %s: 0x%02x\n", __func__, id);

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
}
 
// -------------------------------------
void
eepromInit (void)
{
    EEPROM.begin (EEPROM_SIZE);
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

