#include <stdio.h>
#include <stdlib.h> //malloc, ...
#include <string.h> //memcpy, ...

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_type_list.h"  //list definitions

list_node_t *__list_node_clone_node__(list_node_t *self, list_t *master, 
                                      list_node_t *prev, list_node_t *next)
{
  if(!self || !master)
    return false;

  list_node_t *copy = (list_node_t*)malloc(sizeof(list_node_t));
  init_list_node(master, copy, prev, next, self->container, self->container_sz);

  return copy;
}

bool list_node_extract_container(list_node_t *node, void *outData, size_t *outSize, size_t actualContainerSize)
{
  //exit because node was NULL
  if(!node)
    return false; //error

  if(outData)
  {
    if(actualContainerSize < node->container_sz)
      return false; //error
    memcpy(outData, node->container, node->container_sz);
  }
    
  //if outSize is NULL, no need to set it
  if(outSize)
    *outSize = node->container_sz;

  return true;
}

void init_list_node(list_t *master, list_node_t *node, list_node_t *prev, 
                    list_node_t *next, void *elem, size_t size)
{
  //sanity check
  if(!node) 
    return; //error
  
  //initialize
  node->master = master;
  node->prev = prev;
  node->next = next;

  //memcpy the container data
  node->container = (u8int*)malloc(size);
  memcpy(node->container, elem, size);

  node->container_sz = size;

  //the reserve will be NULL in this case
  node->reserve = NULL;
  node->reserve_sz = 0;

  //internal list functions
  node->__clone_node__ = __list_node_clone_node__;

  //assign corresponding functions
  node->extract_container = list_node_extract_container;
  
  return;
}

/***********************************************
 *                                             *
 * Internal List Functions                     *
 *                                             *
 ***********************************************/

list_node_t *__list_get_node_at__(list_t *self, u32int index)
{
  //Sanity checks
  if(!self || !self->size)
    return false; //error

  list_node_t *node;
  u32int i;

  //for efficiency, if index is near the begining of the list, start from the front
  // else start from the back and work way to front
  if(index / (float)self->size <= 0.5) //start from front
  {
    node = self->head;

    for(i = 0; node && i < index; i++, node = node->next);

  }else{ //start from the end
    node = self->tail;

    for(i = self->size - 1; node && i > index; i--, node = node->prev);
  }

  return node;
}

/***********************************************
 *                                             *
 * Internal List Functions                     *
 *                                             *
 ***********************************************/


/***********************************************
 *                                             *
 * External List Functions                     *
 *                                             *
 ***********************************************/

//gets the first element of the list
bool list_front(list_t *self, void *outData, size_t *outSize, size_t actualContainerSize)
{
  if(!self)
    return false;

  //simply use at on the 0'th element
  return self->at(self, 0, outData, outSize, actualContainerSize);
}

//gets the last element of the list
bool list_back(list_t *self, void *outData, size_t *outSize, size_t actualContainerSize)
{
  if(!self)
    return false;

  //simply use at on the (size - 1)'th element
  // if the size happened to be 0, (0 - 1) will produce the largest number a 32 bit
  // integer can hold, which is larger than 0, that will be caught and handled properly internally
  return self->at(self, self->size - 1, outData, outSize, actualContainerSize);  
}

//creates a list node with contents *elem and pushes it to the front of the list
bool list_push_front(list_t *self, void *elem, size_t size)
{
  //Sanity checks
  if(!self || !elem)
    return false; //error

  //malloc for a node
  list_node_t *node = (list_node_t*)malloc(sizeof(list_node_t));

  //this list is empty, make the head
  if(!self->head || !self->tail)
  {
    init_list_node(self, node, NULL, NULL, elem, size);

    //the head and tail will be equal
    self->head = self->tail = node;
  }else{
    /*initialize a node with
     * the prev node being NULL
     * the next node being the current head of self*/
    init_list_node(self, node, NULL, self->head, elem, size);    

    //on the old head of the list, make the prev this node
    self->head->prev = node;

    //make the head of the list this node
    self->head = node;
  }

  //increment self's size
  self->size++;

  //sucess!
  return true;
}

//creates a list node with contents *elem and pushes it to the back of the list
bool list_push_back(list_t *self, void *elem, size_t size)
{
  //Sanity checks
  if(!self || !elem)
    return false; //error

  //malloc for a node
  list_node_t *node = (list_node_t*)malloc(sizeof(list_node_t));

  //this list is empty, make the head
  if(!self->head || !self->tail)
  {
    init_list_node(self, node, NULL, NULL, elem, size);

    //the head and tail will be equal
    self->head = self->tail = node;
  }else{
    /*initialize a node with
     * the prev node being the tail of self
     * the next node being NULL*/
    init_list_node(self, node, self->tail, NULL, elem, size);    

    //on the old tail of the list, make the next this node
    self->tail->next = node;

    //make the tail of the list this node
    self->tail = node;
  }

  //increment self's size
  self->size++;

  //sucess!
  return true;
}

//removes the first element in the list self
bool list_pop_front(list_t *self)
{
  //sanity checks
  if(!self || !self->tail)
    return false;

  list_node_t *node = self->head;
  
  //set the head now to the node's next
  self->head = node->next;
  //the head's prev has to be cleared
  self->head->prev = NULL;

  //decrement the size
  self->size--;

  //if the size is 0, make the tail of the list NULL also
  if(!self->size)
    self->tail = NULL;

  //free the container and the node itself
  free(node->container);
  free(node);

  return true;
}

//removes the last element in the list self
bool list_pop_back(list_t *self)
{
  //sanity checks
  if(!self)
    return false;

  //not exactly and error, just nothing to pop
  if(!self->tail)
    return true;

  list_node_t *node = self->tail;
  
  //set the tail now to the node's prev
  self->tail = node->prev;
  //the tail's next has to be cleared
  self->tail->next = NULL;

  //decrement the size
  self->size--;

  //if the size is 0, make the head of the list NULL also
  if(!self->size)
    self->head = NULL;

  //free the container and the node itself
  free(node->container);
  free(node);

  return true;
}

//at the list self, assigns outData the contents of the node at index index
//
//actualContainerSize is the size of the container behind the void*
// this function will detect if it is too small and error out
bool list_at(list_t *self, u32int index, void *outData, size_t *outSize, size_t actualContainerSize)
{
  //santiy checks
  if(!self || index >= self->size)
    return false; //error

  list_node_t *node = self->__get_node_at__(self, index);

  //sanity check
  if(!node)
     return false; //error

  //node is now the node at the desired index, extract its information
  return node->extract_container(node, outData, outSize, actualContainerSize);
}

void *list_get(list_t *self, u32int index, size_t *outSize)
{
  //santiy checks
  if(!self || index >= self->size)
    return NULL; //error

  list_node_t *node = self->__get_node_at__(self, index);

  if(!node)
    return NULL; //error

  if(outSize)
    *outSize = node->container_sz;

  return node->container;
}

//inserts element 'void *elem' with size 'size_t size' at index 'u32int index' in self
bool list_insert(list_t *self, u32int index, void *elem, size_t size)
{
  //sanity checks
  if(!elem || !self || !size || index > self->size)
    return false; //error

  //handle the end cases differently
  // use push front/back if the index is the beginning/end
  if(!index) //index is the first element
    return self->push_front(self, elem, size);
  else if(index == self->size) //index is past the last element
    return self->push_back(self, elem, size);
  
  //The index is somewhere inside of the list
    
  //index - 1 will get the node in the list that will preceed this inserted node
  // prev_node will be guaranteed to have a next node and
  // next will be guaranteed to have a prev node
  // as the end cases were handled in the above if statements
  list_node_t *prev_node = self->__get_node_at__(self, index - 1);

  //sanity checks
  if(!prev_node || !prev_node->next || !prev_node->next->prev)
    return false; //error

  list_node_t *next_node = prev_node->next;

  //create this node
  list_node_t *node = (list_node_t*)malloc(sizeof(list_node_t)); //malloc for a node
  init_list_node(self, node, prev_node, next_node, elem, size);

  //break the link between prev and next and insert node between them
  prev_node->next = next_node->prev = node;

  //increment the self's size
  self->size++;

  //sucess!
  return true;

}

//inserts n_elements into the list from the input array at the index 'index'
bool list_insert_array(list_t *self, u32int index, void *array, size_t elem_sz, u32int n_elements)
{
  //sanity checks
  if(!self || !array || !elem_sz || index > self->size)
    return false; //error

  //if n_elements is 0, not an errer, just an easy job
  if(!n_elements)
    return true;

  //handle the end cases differently
  // use push front/back if the index is the beginning/end
  if(!index) //index is the first element
  {
    //push to the front the first element in the array and insert the rest of the array starting at index 1
    if(!self->push_front(self, array, elem_sz))
      return false;

    //insert the rest of the array at index 1 with n_elements - 1 as the size since the first element
    // was already pushed
    return self->insert_array(self, 1, array + elem_sz, elem_sz, n_elements - 1);
  }else if(index == self->size) //index is past the last element
  {
    //push to the back the last element in the array and insert the rest of the 
    // array starting at index self->size - 1    
    if(!self->push_back(self, array + (n_elements - 1) * elem_sz, elem_sz))
      return false;

    //insert the rest of the array at index self->size - 1 with n_elements - 1 as the size since the 
    // last element was already pushed
    return self->insert_array(self, self->size - 1, array, elem_sz, n_elements - 1);
  }
  
  //loop and insert all of the other elements
  u32int i;
  for(i = 0; i < n_elements; i++)
  {
    if(!self->insert(self, index + i, array + i * elem_sz, elem_sz))
       return false;
  }

  return true;
}

//given a list self that contains nodes and a list ret that is initialized properly
// copies every node and respective container from self and links it in ret
bool list_clone(list_t *self, list_t *ret)
{
  if(!self || !ret)
    return false; //error

  list_node_t *node = self->head;
  list_node_t *prev_node = NULL;

  for(; node; node = node->next)
  {
    list_node_t *insert = node->__clone_node__(node, ret, prev_node, NULL);

    //set the previous node's next as this insert if prev_node exits
    if(prev_node)
      prev_node->next = insert;
    else //insert is the first element, set it as the list's head
      ret->head = insert;

    //before the loop loops, prev_node is set and then node will be set to next,
    // that makes prev node be the previous node in the next loop
    prev_node = insert;
  }
  
  //prev_node is now the last element in the list, set it as the list's tail
  ret->tail = prev_node;

  //copy the size over
  ret->size = self->size;

  //the reserve and function pointers are handled in the initilization of the list

  return true;
}

//frees the list including the nodes and their containers
bool list_free_all(list_t *self)
{
  //sanity check
  if(!self)
    return false; //error

  list_node_t *node = self->head;

  while(node)
  {
    list_node_t *next_node = node->next;

    //free the container and node
    free(node->container);
    free(node);

    //assign the next node to node
    node = next_node;
  }

  //set the head and tail of the list to NULL
  self->head = self->tail = NULL;

  //free the lists reserve, if reserve is NULL, free will exit gracefully
  free(self->reserve);
  
  //sucess!
  return true;
}

void init_list_empty_head(list_t *list)
{
  //Sanity checks
  if(!list)
    return; //error

  //all values to be set to 0
  list->head = list->tail = 0;
  list->size = 0;

  //the reserve will be NULL in this case
  list->reserve = NULL;
  list->reserve_sz = 0;

  //assign internal list functions not meant to be called by the user
  list->__get_node_at__ = __list_get_node_at__;

  //assign corresponding functions
  list->front      = list_front;
  list->back       = list_back;
  list->push_front = list_push_front;
  list->pop_front  = list_pop_front;
  list->push_back  = list_push_back;
  list->pop_back   = list_pop_back;
  list->at         = list_at;
  list->get        = list_get;
  list->insert     = list_insert;
  list->insert_array = list_insert_array;
  list->clone      = list_clone;
  list->free_all   = list_free_all;

  return;
}

/***********************************************
 *                                             *
 * External List Functions                     *
 *                                             *
 ***********************************************/

