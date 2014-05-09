
#include <stdlib.h> //malloc, ...
#include <string.h> //memcpy, ...

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_vector.h"

bool __vector_push_back_data__(vector_t *self, void *data, size_t data_sz)
{
  if(!self || !self->data || !data)
    return false;

  //if the data will not fit, reallocate
  if(self->alloc_sz - self->offset < data_sz)
  {
    //reallocate on a exponential curve VECTOR_BASE_ALLOC * 2**(n_reallocs)
    self->alloc_sz *= 2; 
    self->data = realloc(self->data, self->alloc_sz);

    //if realloc failed, return an error
    if(!self->data)
      return error("Realloc failed, possibly out of memory, attempted array size %d\n", self->alloc_sz);
  }

  //copy the data, self->data should have enough empty space if it has reached here
  memcpy(self->data + self->offset, data, data_sz);

  //sucess
  return true;
}

bool vector_push_back(vector_t *self, void *elem, size_t elem_sz)
{
  if(!self || !self->data || !elem)
    return false;

  assert(self->elem_sz == elem_sz);

  return self->__push_back_data__(self, elem, elem_sz);
}

void init_vector_empty(vector_t *vector, size_t elem_sz)
{
  //set up allocation
  vector->data = (u8int*)malloc(VECTOR_BASE_ALLOC);
  vector->alloc_sz = VECTOR_BASE_ALLOC;
  vector->offset = 0;

  //set up element size
  vector->elem_sz = elem_sz;

  //assign internal function pointers
  vector->__push_back_data__ = __vector_push_back_data__;

  //assign function pointers
  vector->push_back = vector_push_back;
  
  return;
}
