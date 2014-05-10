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
  u32int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  //Test variables

  LIST_INSERT_ARRAY(list, 0, arr + 1, 4);

  LIST_PUSH_BACK(list, test);
  LIST_PUSH_BACK(list, mid);
  LIST_PUSH_BACK(list, end);

  LIST_PUSH_FRONT(list, end);
  LIST_PUSH_FRONT(list, end);
  LIST_PUSH_FRONT(list, end);
  LIST_POP_BACK(list);

  LIST_INSERT(list, 1, mid);

  u32int foreachTmp;
  size_t sizer;

  LIST_FRONT(list, foreachTmp);
  printf("FRONT %d\n", foreachTmp);
  LIST_BACK_WITH_SIZE(list, foreachTmp, sizer);
  printf("BACK %d size %zu\n\n", foreachTmp, sizer);
  
  LIST_FOREACH(foreachTmp, list)
  {
    printf("Test %d\n", foreachTmp);
  }

  list_t copy_list;
  STATIC_TYPE_LIST_INIT(copy_list, u32int);
  LIST_CLONE(list, copy_list);

  printf("-----------------\n");

  size_t gotSZ;
  u32int *got = LIST_GET_WITH_SIZE(copy_list, 4, gotSZ);
  printf("LIST GET %d %zd\n", *got, gotSZ);

  LIST_FREE_ALL(list);
  LIST_FOREACH(foreachTmp, copy_list)
  {
    printf("Test %d\n", foreachTmp);
  }

  printf("----------------- VECTOR\n");

  vector_t vec;
  VECTOR_INIT(vec, u32int);

  VECTOR_PUSH_BACK_ARRAY(vec, arr + 1, 9);

  VECTOR_POP_BACK(vec);

  u32int tmp;
  VECTOR_FRONT(vec, tmp);
  printf("FRONT %d\n", tmp);
  VECTOR_BACK(vec, tmp);
  printf("BACK %d\n\n", tmp);

  VECTOR_RESIZE(vec, 5);
  VECTOR_INSERT(vec, 5, *(arr + 9));
  for(tmp = 0; tmp < 1000; tmp++)
    VECTOR_INSERT_ARRAY(vec, 6, arr + 4, 3);

  VECTOR_ERASE(vec, 1, vec.size);

  vector_t vec2;
  VECTOR_INIT(vec2, u32int);
  VECTOR_PUSH_BACK_ARRAY(vec2, arr, 5);
  VECTOR_SWAP(vec, vec2);

  VECTOR_SHRINK_TO_FIT(vec2);

  u32int *vGot = VECTOR_GET(vec, 2);
  *vGot += 25;
  printf("Vector got %d\n", *vGot);

  u32int var;
  VECTOR_FOREACH(var, vec)
    printf("DATA %d\n", var);

  printf("--------------\n");

  VECTOR_FOREACH(var, vec2)
    printf("DATA %d\n", var);

  printf("Size %zu %zuKB \t n_elements %d\n", vec2.alloc_sz, vec2.alloc_sz / 1024, vec2.size);

  VECTOR_FREE_ALL(vec);

  return 0;
}
