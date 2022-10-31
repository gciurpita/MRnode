#ifndef WIFI_H
# define WIFI_H

void wifiConnect (void);
void nodeConnect (void);
void wifiMonitor (void);
void wifiReceive (void);
void wifiReset   (void);
void wifiSend    (const char* s);
#endif
