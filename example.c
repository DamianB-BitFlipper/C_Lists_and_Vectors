
#include <stdio.h> //printf, ...

#include "utils.h"            //special types, ie) u32int, u8int
#include "static_type_list.h" //list of static type nodes
#include "dynamic_vector.h"   //dynamic size vector (array)

#define USE_UI

//default values for non-ui mode
#define BENCHMARK_SZ  10000000 //default size for list/vector if ui mode is off
#define BENCHMARK_DEMO 'V'   //default benchmark type
#define BENCHMARK_PRINT false //wheather it will print the elements in the list/vector

s32int main()
{
  for(;;)
  {
    char demo;
    // 'L' for list demo
    // 'V' for vector demo

#ifdef USE_UI
    printf("List (L) or Vector (V)\nChoose a demo: ");
    scanf("%c", &demo);
#else
    demo = BENCHMARK_DEMO;
#endif

    if(demo == 'L') //example usage of list
    {
      u32int size, i;

#ifdef USE_UI
      printf("Enter a list size: ");
      scanf("%u", &size); //read input
#else
      size = BENCHMARK_SZ;
#endif

      //initialize a static type list with all elements of type uint64
      list_t list;
      STATIC_TYPE_LIST_INIT(list, uint64);

      for(i = 1; i < size + 1; i++)
        LIST_EMPLACE_BACK(list, uint64, i * i); //emplace back all squares of i

      //sum the values up
      uint64 var, sum = 0;
      LIST_FOREACH(var, list)
        sum += var;

      if(BENCHMARK_PRINT)
      {
        //print each value
        LIST_FOREACH(var, list)
          printf("%lld, ", var);
        printf("\n");
      }

      printf("The sum of all elements in list is %lld\n", sum);

      //free the list
      LIST_FREE_ALL(list);

      //sucess!
      return 0;
    }else if(demo == 'V') //example usage of a vector
    {
      u32int size, i;

#ifdef USE_UI
      printf("Enter a vector size: ");
      scanf("%u", &size);
#else
      size = BENCHMARK_SZ;
#endif

      //initialize a vector with elements of type uint64
      vector_t vec;
      VECTOR_INIT(vec, uint64);

      //reserve space for i elements for efficiency
      VECTOR_RESERVE(vec, i);
      
      for(i = 1; i < size + 1; i++)
        VECTOR_EMPLACE_BACK(vec, uint64, i * i); //emplace back all squares of i

      //sum the values up
      uint64 var, sum = 0;
      VECTOR_FOREACH(var, vec)
        sum += var;

      if(BENCHMARK_PRINT)
      {
        //print each value
        VECTOR_FOREACH(var, vec)
          printf("%lld, ", var);
        printf("\n");
      }

      printf("The sum of all elements in vector is %lld\n", sum);

      //free the vector
      VECTOR_FREE_ALL(vec);

      //sucess!
      return 0;
    }
  }
}


