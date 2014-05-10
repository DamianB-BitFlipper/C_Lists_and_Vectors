
#include <stdlib.h> //malloc, ...
#include <string.h> //memcpy, ...

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_vector.h"

//internal function to push an arbitrary size data to the vector, 
// data_sz must be a multiple of self->elem_sz
bool __vector_push_back_data__(vector_t *self, void *data, size_t data_sz)
{
  if(!self || !self->data || !data)
    return false;

  //the size of data must be a multiple of the self->elem_sz, else error
  if(data_sz % self->elem_sz)
    return error("Vectore assigning data of size %d with element size %d cannot be done", data_sz, self->elem_sz);

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

  //increase the offset and the size
  self->offset += data_sz;
  self->size += data_sz / self->elem_sz; 

  //sucess
  return true;
}

//vector is made to hold n elements, if it holds more, the elements on the end will be destroyed
bool vector_resize(vector_t *self, size_t n)
{
  if(!self)
    return false;

  self->alloc_sz = n * self->elem_sz;

  //if alloc_sz is 0, make it fit atleast one element
  if(!self->alloc_sz)
    self->alloc_sz = self->elem_sz;

  //reallocate memory
  self->data = realloc(self->data, self->alloc_sz);

  //if realloc failed, return an error
  if(!self->data)
    return error("Realloc failed, possibly out of memory, attempted array size %zu\n", self->alloc_sz);

  //if n is smaller than the size, make the size equal n
  if(n < self->size)
    self->size = n;

  return true;
}

//requests that vector can contain atleast n elements
bool vector_reserve(vector_t *self, size_t n)
{
  if(!self)
    return false;

  //no need to resize, exit happily
  if(n * self->elem_sz <= self->alloc_sz)
    return true;
  else //resize
    return self->resize(self, n);
}

//gets the data at an index in the vector
bool vector_at(vector_t *self, u32int index, void *ret, size_t ret_sz)
{
  if(index >= self->size || !self || !self->data)
    return false;
  
  if(self->elem_sz != ret_sz)
    return error("Vector cannot extract element of size %d into %d, sizes must equal\n", self->elem_sz, ret_sz);

  memcpy(ret, self->data + index * self->elem_sz, self->elem_sz);
  return true;
}

//gets the first element from the vector
bool vector_front(vector_t *self, void *ret, size_t ret_sz)
{
  if(!self)
    return false;

  return self->at(self, 0, ret, ret_sz);
}

//gets the last element from the vector
bool vector_back(vector_t *self, void *ret, size_t ret_sz)
{
  if(!self)
    return false;

  //simply use at on the (size - 1)'th element
  // if the size happened to be 0, (0 - 1) will produce the largest number a 32 bit
  // integer can hold, which is larger than 0, that will be caught and handled properly internally
  return self->at(self, self->size - 1, ret, ret_sz);
}

//pushes back one element to vector self
bool vector_push_back(vector_t *self, void *elem, size_t elem_sz)
{
  if(!self || !self->data || !elem)
    return false;

  assert(self->elem_sz == elem_sz);

  return self->__push_back_data__(self, elem, elem_sz);
}

//n_elements is the amount of elements in the array, elem_sz is the size of one of those elements
bool vector_push_back_array(vector_t *self, void *array, u32int n_elements, size_t elem_sz)
{
  if(!self || !array || !elem_sz)
    return false; //error

  //push back 0 elements, easy!
  if(!n_elements)
    return true;

  assert(self->elem_sz == elem_sz);

  return self->__push_back_data__(self, array, n_elements * elem_sz);
}

//removes the last element in the vector self
bool vector_pop_back(vector_t *self)
{
  if(!self)
    return false;

  //boundary check
  if(!self->size || !self->offset)
    return true; //nothing to pop back, easy sucess!

  //simply subtract elem_sz from the offset and decrement the size
  self->offset -= self->elem_sz;
  self->size--;
  return true;
}

//frees all of the data of the vector
bool vector_free_all(vector_t *self)
{
  if(!self)
    return false;

  //free the data and clear everything
  free(self->data);
  memset(self, 0x0, sizeof(vector_t));

  return true;
}

//initializes a vector with function pointers, etc...
void init_vector_empty(vector_t *vector, size_t elem_sz)
{
  //set up allocation
  vector->data = (u8int*)malloc(VECTOR_BASE_ALLOC);
  vector->alloc_sz = VECTOR_BASE_ALLOC;
  vector->offset = 0;

  //set up element size
  vector->elem_sz = elem_sz;
  vector->size = 0;

  //assign internal function pointers
  vector->__push_back_data__ = __vector_push_back_data__;

  //assign function pointers
  vector->resize    = vector_resize;
  vector->reserve   = vector_reserve;
  vector->front     = vector_front;
  vector->back      = vector_back;
  vector->push_back = vector_push_back;
  vector->push_back_array = vector_push_back_array;
  vector->pop_back  = vector_pop_back;
  vector->at        = vector_at;
  vector->free_all  = vector_free_all;
  return;
}
