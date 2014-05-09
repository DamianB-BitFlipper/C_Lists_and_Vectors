#include <stdio.h>

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_type_list.h" //static list definitions
#include "static_type_list.h" //static list definitions

//compile: gcc -o list_implementation dynamic_type_list.c static_type_list.c util_main.c -O3 -Wall -std=c99

s32int main()
{
  list_t list;
  STATIC_TYPE_LIST_INIT(list, u32int);

  //Test variables
  u32int test = 100, mid = 200, end = 300;

  struct sTest_s
  {
    u32int test;
    uint64 wow;
    u32int to;
  } sTest;

  sTest.test = sTest.wow = sTest.to = 10;
  //Test variables

  LIST_PUSH_BACK(list, test);
  LIST_PUSH_BACK(list, mid);
  LIST_PUSH_BACK(list, end);

  LIST_PUSH_FRONT(list, end);
  LIST_PUSH_FRONT(list, end);
  LIST_PUSH_FRONT(list, end);
  LIST_POP_BACK(list);

  LIST_INSERT(list, 1, mid);

  u32int foreachTmp;
  LIST_FOREACH(foreachTmp, list)
  {
    printf("Test %d\n", foreachTmp);
  }

  list_t copy_list;
  STATIC_TYPE_LIST_INIT(copy_list, u16int);
  LIST_CLONE(list, copy_list);

  //TODO add error() function

  printf("-----------------\n");

  LIST_FREE_ALL(list);
  LIST_FOREACH(foreachTmp, copy_list)
  {
    printf("Test %d\n", foreachTmp);
  }

  /*u32int nice, in = 2;
  LIST_AT(list, in, nice);
  printf("At %d is %d size %d\n", in, nice, list.size);

  printf("HEAD %d TAIL %p:%d\n", 
         *(u32int*)(list.head->next->next->prev->container), 
         (list.tail->container), *(u32int*)(list.tail->container));*/

  return 0;
}

