#ifndef WIFI_H
# define WIFI_H

extern int  dbgWifi;

const unsigned STR_SIZE = 40;

extern char host [STR_SIZE];
extern char ssid [STR_SIZE];
extern char pass [STR_SIZE];

// ----------------------------------------
void  wifiConnect (void);
void  nodeConnect (void);
void  wifiIpAdd   (char *ip);
void  wifiIpClr   (void);
char *wifiIpGet   (int idx);
void  wifiIpList  (void);
void  wifiMonitor (void);
void  wifiReceive (void);
void  wifiReset   (void);
void  wifiSend    (const char* s);
#endif
