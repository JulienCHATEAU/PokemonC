#ifndef TTYRAW_H
#define TTYRAW_H

void tty_atexit(void);
int tty_reset(void);
void tty_raw(void);
int screenio(void);
void fatal(char *mess);
int setRawMode(char on);

#endif