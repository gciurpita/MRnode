#ifndef NODE_H
# define NODE_H

enum { ERR_NONE, ERR_NO_HOST, ERR_NO_SSID, ERR_NO_PASS };

extern const char *version;
extern const char *name;
extern       char *s;
extern int         error;

#endif
