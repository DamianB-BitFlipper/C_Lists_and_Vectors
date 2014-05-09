
#include <stdarg.h>  //variable arguments
#include <stdio.h>   //convert formate to string

#include "utils.h"  //special types, ie) u32int, u8int

bool OutputFError(const char *format, ...)
{
  const u32int buffer_sz = 1024;
  char buffer[buffer_sz];

  va_list args; //initialize args
  va_start(args, format);

  //interpret format args
  vsnprintf(buffer, buffer_sz, format, args);

  //print the errors
  printf("ERROR: %s", buffer);

  //free args
  va_end(args);

  //return false as this is an error
  return false;
}
