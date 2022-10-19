#ifndef EEPROM_H
# define EEPROM_H

# include <Arduino.h>

enum {
    ID_END      = 0,

    ID_HOSTNAME = 10,
    ID_SSID     = 11,
    ID_PASSWORD = 12,
    ID_MASTER   = 13,

    ID_IGNORE   = 0xFF,
    ID_NOT_FOUND = -1
};

int  eepromAddr  (byte type);
void eepromClear (void);
void eepromInit  (void);
void eepromRead  (byte type, char *buf, int bufSize );
void eepromScan  (void);
void eepromWrite (byte type, const char *text );
#endif
