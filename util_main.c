#include <stdio.h>

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_type_list.h" //dynamic list definitions
#include "static_type_list.h"  //static list definitions
#include "dynamic_vector.h"  //dynamic vector definitions

//compile: gcc -o list_implementation dynamic_type_list.c static_type_list.c util_main.c utils.c -O3 -Wall -std=c99

s32int main()
{
  list_t list;
  STATIC_TYPE_LIST_INIT(list, u32int);

  //Test variables
  u32int test = 100, mid = 200, end = 300;
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
  STATIC_TYPE_LIST_INIT(copy_list, u32int);
  LIST_CLONE(list, copy_list);

  printf("-----------------\n");

  LIST_FREE_ALL(list);
  LIST_FOREACH(foreachTmp, copy_list)
  {
    printf("Test %d\n", foreachTmp);
  }

  printf("----------------- VECTOR\n");

  return 0;
}
