
#ifndef VECTOR_H
#define VECTOR_H

#include "utils.h"  //special types, ie) u32int, u8int

//base allocation durring initialization is 1KB
#define VECTOR_BASE_ALLOC 1024

/*********Vector macros*********/

//!!! ALL MACROS TAKE NON-POINTER ARGS !!!//

/*initializes a vector*/
#define VECTOR_INIT(vector, type) init_vector_empty(&vector, sizeof(type))

/*resize*/
#define VECTOR_RESIZE(vector, n) vector.resize(&vector, n)

/*reserve*/
#define VECTOR_RESERVE(vector, n) vector.reserve(&vector, n)

/*front*/
#define VECTOR_FRONT(vector, out) vector.front(&vector, &out, sizeof(out))

/*back*/
#define VECTOR_BACK(vector, out) vector.back(&vector, &out, sizeof(out))

/*push back*/
#define VECTOR_PUSH_BACK(vector, elem) vector.push_back(&vector, &elem, sizeof(elem))

/*push back an array*/
#define VECTOR_PUSH_BACK_ARRAY(vector, array, n_elements)               \
  vector.push_back_array(&vector, &array, n_elements, sizeof(*array)) 

/*pop back*/
#define VECTOR_POP_BACK(vector) vector.pop_back(&vector)

/*at*/
#define VECTOR_AT(vector, index, out) vector.at(&vector, index, &out, sizeof(out))

/*free all*/
#define VECTOR_FREE_ALL(vector) vector.free_all(&vector)

/*foreach loop, see dynamic type list for more detail on explanation*/
#define VECTOR_FOREACH(var, vector)                                     \
  for(u32int __index__ = 0;                                             \
      __index__ < vector.size && vector.at(&vector, __index__, &var, sizeof(var)); \
      __index__++)

/*********Vector macros*********/

typedef struct vector_s
{
  u8int *data;      //the pointer to the data
  size_t alloc_sz;  //the size allocated
  u32int offset;    //the offset from the beginning of data to unused data in bytes

  size_t elem_sz;   //the size of one element in the vector in bytes
  u32int size;      //the number of elements in this vector

  //Internal function pointers
  bool (*__push_back_data__)(struct vector_s *self, void *data, size_t data_sz);

  //Function pointers
  bool (*resize)(struct vector_s *self, size_t n);
  bool (*reserve)(struct vector_s *self, size_t n);
  bool (*front)(struct vector_s *self, void *ret, size_t ret_sz);
  bool (*back)(struct vector_s *self, void *ret, size_t ret_sz);
  bool (*push_back)(struct vector_s *self, void *elem, size_t elem_sz);
  bool (*push_back_array)(struct vector_s *self, void *array, u32int n_elements, size_t elem_sz);
  bool (*pop_back)(struct vector_s *self);
  bool (*at)(struct vector_s *self, u32int index, void *ret, size_t ret_sz);
  bool (*free_all)(struct vector_s *self);
} vector_t;

//initializes an empty vector
void init_vector_empty(vector_t *vector, size_t elem_sz);

#endif //VECTOR_H