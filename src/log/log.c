#include "log.h"
#include <stdio.h>
#include <stdarg.h>

int LOG_init(void)
{
  return 0;
}

void LOG_quit(void)
{

}

int LOG_msg(const char *fmt, ...)
{
  int status;
  va_list args;

  status = 0;
  va_start(args, fmt);

  vprintf(fmt, args);
  
  va_end(args);

  return status;
}