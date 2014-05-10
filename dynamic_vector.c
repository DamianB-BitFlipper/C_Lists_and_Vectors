
#include <stdlib.h> //malloc, ...
#include <string.h> //memcpy, ...

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_vector.h"

bool __vector_realloc_if_necessary__(vector_t *self, size_t data_sz)
{
  if(!self)
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
  return true;
}

//internal function to push an arbitrary size data to the vector, 
// data_sz must be a multiple of self->elem_sz
bool __vector_push_back_data__(vector_t *self, void *data, size_t data_sz)
{
  if(!self || !self->data || !data)
    return false;

  //the size of data must be a multiple of the self->elem_sz, else error
  if(data_sz % self->elem_sz)
    return error("Vectore assigning data of size %d with element size %d cannot be done", 
                 data_sz, self->elem_sz);

  //check if we have to realloc and do so if necessary
  if(!self->__realloc_if_necessary__(self, data_sz))
    return false;

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
  // decrement the offset respectivly
  if(n < self->size)
  {
    self->size = n;
    self->offset = n * self->elem_sz;
  }

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

//shrinks the container size to only fit the elements in the vector
// frees unused memory
bool vector_shrink_to_fit(vector_t *self)
{
  if(!self)
    return false;
  
  self->resize(self, self->size);
  return true;
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

//returns the pointer to the direct memory location of the element at the index 'index'
void *vector_get(vector_t *self, u32int index)
{
  //santiy checks
  if(!self || !self->data || index >= self->size)
    return NULL; //error

  return self->data + index * self->elem_sz;
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

//elem will be inserted at index and all elements after it will be pushed back
// elem can also be an array with elem_sz being the size of the array in bytes
bool vector_insert(vector_t *self, u32int index, void *elem, size_t elem_sz)
{
  if(!self || !self->data || !elem || index > self->size)
    return false;

  assert(!(elem_sz % self->elem_sz) && 
         "The element size of void *elem must be a multiple of the element size in the vector");

  //check if we have to realloc and do so if necessary
  if(!self->__realloc_if_necessary__(self, elem_sz))
    return false;

  /*move all elements at index and past back elem_sz
   *
   * 'index * self->elem_sz' gets the amount of bytes before
   *   where elem will be placed
   *
   * Destination: 'index * self->elem_sz + elem_sz' offsets
   *                the data to the address directly after the end of
   *                where elem will be placed, that is where the rest of the vector
   *                can be copied to
   *
   * Source: 'index * self->elem_sz' offsets the data to the address of the data
   *           that will be pushed backwards when elem is inserted
   *
   * Size: 'self->offset - index * self->elem_sz', take all of the data and
   *         subtract what will not be pushed backwards gives the size of the data that 
   *         will be pushed back
   *
   * It is guaranteed that there is enough space after the offset to hold elem_sz
   *  because the function call __realloc_if_necessary__() handles that*/
  memmove(self->data + index * self->elem_sz + elem_sz,
          self->data + index * self->elem_sz, 
          self->offset - index * self->elem_sz);
  
  //copy the data to its location
  memcpy(self->data + index * self->elem_sz, elem, elem_sz);

  //increment the size and offset
  self->offset += elem_sz;
  self->size += elem_sz / self->elem_sz; 

  //sucess!
  return true;
}

//elements from start to end (not including end) will be removed and the rest of the
// elements including and following end shifted leftward
bool vector_erase(vector_t *self, u32int start, u32int end)
{
  if(!self || !self->data || start > end || end > self->size)
    return false;

  //if start == end, not an error, just exit easily
  if(start == end)
    return true;

  //do the memmove here backwards and decrement size and offset
  memmove(self->data + start * self->elem_sz,
          self->data + end * self->elem_sz, 
          self->offset - end * self->elem_sz);

  //decrement the offset and size
  // the offset is decremented by the number of elements taken out times the size of each
  // the size is decremenented by the number of elements taken out
  self->offset -= (end - start) * self->elem_sz;
  self->size -= end - start;

  //sucess
  return true;
}

//swaps the contents of two vectors
bool vector_swap(vector_t *one, vector_t *two)
{
  if(!one || !two)
    return false;

  //a temporary vector
  vector_t vTmp;

  //copy the contents of one into the tmp vector
  memcpy(&vTmp, one, sizeof(vector_t));
  
  //copy the contents of two into one
  memcpy(one, two, sizeof(vector_t));

  //copy the contents in the tmp vector (one's contents) into two
  memcpy(two, &vTmp, sizeof(vector_t));

  return true;
}

//clears only the size and offset variables
bool vector_clear(vector_t *self)
{
  if(!self)
    return false;

  //set the size and offset to 0, reallocation is not necessary
  self->size = self->offset = 0;
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
  vector->__realloc_if_necessary__ = __vector_realloc_if_necessary__;
  vector->__push_back_data__ = __vector_push_back_data__;

  //assign function pointers
  vector->resize    = vector_resize;
  vector->reserve   = vector_reserve;
  vector->shrink_to_fit = vector_shrink_to_fit;
  vector->front     = vector_front;
  vector->back      = vector_back;
  vector->push_back = vector_push_back;
  vector->push_back_array = vector_push_back_array;
  vector->pop_back  = vector_pop_back;
  vector->at        = vector_at;
  vector->get       = vector_get;
  vector->insert    = vector_insert;
  vector->erase     = vector_erase;
  vector->swap      = vector_swap;
  vector->clear     = vector_clear;
  vector->free_all  = vector_free_all;
  return;
}
