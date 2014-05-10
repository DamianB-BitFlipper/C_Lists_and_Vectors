/*
 * This file is an example of a list that
 * is derived from the dynamic type list in dynamic_type_list.c
 *
 * This list type defines a list that is of a static type once initialized
 */

#include <stdlib.h> //malloc, ...
#include <string.h> //memcpy, ...

#include "utils.h"  //special types, ie) u32int, u8int
#include "static_type_list.h"

//all functions add a simple size check
bool static_type_list_push_front(list_t *self, void *elem, size_t size)
{
  if(!self || !self->reserve)
    return false;

  assert(((static_list_reserve_t*)self->reserve)->element_sz == size &&
    "Cannot insert an element that does not have a size equal to the others in the list");

  return list_push_front(self, elem, size);
}

bool static_type_list_push_back(list_t *self, void *elem, size_t size)
{
  if(!self || !self->reserve)
    return false;

  assert(((static_list_reserve_t*)self->reserve)->element_sz == size &&
    "Cannot insert an element that does not have a size equal to the others in the list");

  return list_push_back(self, elem, size);
}

bool static_type_list_insert(list_t *self, u32int index, void *elem, size_t size)
{
  if(!self || !self->reserve)
    return false;

  assert(((static_list_reserve_t*)self->reserve)->element_sz == size &&
    "Cannot insert an element that does not have a size equal to the others in the list");
  
  return list_insert(self, index, elem, size);
}

bool static_type_list_clone(list_t *self, list_t *ret)
{
  //a bunch of sanity checks
  // clone only if self and ret are static type lists and
  // the size of the elements they hold is the same
  if(!self || !self->reserve || 
     !ret  || !ret->reserve)
    return false;
  
  if(self->reserve_sz != ret->reserve_sz)
    return error("Lists do not match self:%d != ret:%d\n", self->reserve_sz, ret->reserve_sz);
  
  if(((static_list_reserve_t*)self->reserve)->element_sz != ((static_list_reserve_t*)ret->reserve)->element_sz)
    return error("List node size is not of same type (same size in bytes) self:%d != ret:%d\n", 
                 ((static_list_reserve_t*)self->reserve)->element_sz, 
                 ((static_list_reserve_t*)ret->reserve)->element_sz); 

  //if the sanity checks passed, clone like a normal list
  return list_clone(self, ret);
}

void init_static_type_list_empty_head(list_t *list, size_t elem_sz)
{
  //Sanity checks
  if(!list)
    return; //error

  //call the dynamic type list's (parent class)'s initializator
  init_list_empty_head(list);

  //set the reserve properly, since element_sz is a constant member
  // it has to be initilized like so and then copied over to avoid the
  // compiler from complaining
  static_list_reserve_t reserve = { .element_sz = elem_sz};

  list->reserve = (void*)malloc(sizeof(static_list_reserve_t));
  memcpy(list->reserve, &reserve, sizeof(static_list_reserve_t));
  list->reserve_sz = sizeof(static_list_reserve_t);

  //assign corresponding functions
  // only functions that were defined above are needed to be changed
  list->push_front = static_type_list_push_front;
  list->push_back  = static_type_list_push_back;
  list->insert     = static_type_list_insert;
  list->clone      = static_type_list_clone;

  return;
}
