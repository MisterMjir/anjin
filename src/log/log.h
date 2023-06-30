#ifndef ANJIN_LOG_H
#define ANJIN_LOG_H

int  LOG_init(void);
void LOG_quit(void);
int  LOG_msg(const char *fmt, ...);

#endif