

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h> //bool type
#include <assert.h> //assert, ...

//error function
#define error OutputFError
bool OutputFError(const char *format, ...);

typedef unsigned long long int uint64;
typedef          long long int sint64;
typedef unsigned int u32int;
typedef          int s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char u8int;
typedef          char s8int;


#endif //UTILS_H
