#ifndef NODE_H
# define NODE_H

# undef EZSBC
# define TTGO
# define I2C

enum {
    ERR_NONE,
    ERR_NO_HOST,
    ERR_NO_SSID,
    ERR_NO_PASS,
    ERR_NO_NET,
};

extern const char   *version;
extern unsigned      debug;
extern int           error;
extern int           ledMode;
extern unsigned long msec;
extern char          s [];

#endif
