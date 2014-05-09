
#ifndef VECTOR_H
#define VECTOR_H

#include "utils.h"  //special types, ie) u32int, u8int

//base allocation durring initialization is 1KB
#define VECTOR_BASE_ALLOC 1024

/*********Vector macros*********/

//!!! ALL MACROS TAKE NON-POINTER ARGS !!!//

/*initializes a vector*/
#define VECTOR_INIT(vector, type) init_vector_empty(&vector, sizeof(type))

/*push back*/
#define VECTOR_PUSH_BACK(vector, elem) vector.push_back(&vector, &elem, sizeof(elem))

/*********Vector macros*********/

typedef struct vector_s
{
  u8int *data;      //the pointer to the data
  size_t alloc_sz;  //the size allocated
  u32int offset;    //the offset from the beginning of data to unused data in bytes

  size_t elem_sz;   //the size of one element in the vector
  
  //Internal function pointers
  bool (*__push_back_data__)(struct vector_s *self, void *data, size_t data_sz);

  //Function pointers
  bool (*push_back)(struct vector_s *self, void *elem, size_t elem_sz);

} vector_t;

//initializes an empty vector
void init_vector_empty(vector_t *vector, size_t elem_sz);

#endif //VECTOR_H
