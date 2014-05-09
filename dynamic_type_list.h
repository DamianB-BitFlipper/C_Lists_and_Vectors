

#ifndef LIST_IMPLEMENTATION_H
#define LIST_IMPLEMENTATION_H

#include "utils.h"  //special types, ie) u32int, u8int

/*********List macros*********/

//!!! ALL MACROS TAKE NON-POINTER ARGS !!!//

/*initializes a list*/
#define LIST_INIT(list) init_list_empty_head(&list);

/*push front*/
#define LIST_PUSH_FRONT(list, var) list.push_front(&list, &var, sizeof(var))

/*pop front*/
#define LIST_POP_FRONT(list) list.pop_front(&list)

/*push back*/
#define LIST_PUSH_BACK(list, var) list.push_back(&list, &var, sizeof(var))

/*pop back*/
#define LIST_POP_BACK(list) list.pop_back(&list)

/*at*/
//the variable out gets the contents of the node's container
#define LIST_AT(list, index, out) list.at(&list, index, &out, 0, sizeof(out))

//same as LIST_AT, with the variable size getting the size of the node's data
#define LIST_AT_WITH_SIZE(list, index, out, size) list.at(&list, index, &out, &size, sizeof(out)) 

/*insert*/
#define LIST_INSERT(list, index, var) list.insert(&list, index, &var, sizeof(var))

/*clone list*/
#define LIST_CLONE(list, clone_list) list.clone(&list, &copy_list);

/*free all*/
#define LIST_FREE_ALL(list) list.free_all(&list)

/*foreach
 * In struct {...} s:
 *            __typeof__(var) *x is a pointer with the type of var
 *                                  that is initialized with the address of var
 *            list_node_t *node is a pointer to the node in the list
 *                                  that is initialized with the list's head
 *                                  every time the for loop loops, its value is updated
 *                                  with the next node in the list
 * Condition:
 *            while, s.node (the node being iterated) is not NULL, ie) a valid node
 *            and, the extract_container() of the node into the pointer s.x has not failed
 *                     this extract_container function is called every time the condition is executed
 *                     and the value of s.x (*s.x) attains the value of the value container of that node
 *                     As *s.x attains a value, that indirecly makes var attain the same value as
 *                     they were initilized with the same address
 * Iteration:
 *            The next node is assigned the pointer of the current node's next pointer
 *
 * !!! Declaration of var happens outside of LIST_FOREACH loop !!!
 * */
#define LIST_FOREACH(var, list)                                         \
  for(struct { __typeof__(var) *x; list_node_t *node; } s = {&var, list.head}; \
      s.node != NULL && (s.node->extract_container(s.node, s.x, 0, sizeof(var))); \
      s.node = s.node->next)

/*********List macros*********/

//a node in a list
typedef struct list_node_s
{
  //points the the master list struct
  struct list_s *master;

  struct list_node_s *prev, *next;

  u8int *container;
  size_t container_sz;

  //reserved pointer of any extensions
  // can be used by assigning a struct that extends this struct
  void *reserve;
  size_t reserve_sz;
  
  //internal list functions
  struct list_node_s *(*__clone_node__)(struct list_node_s*, struct list_s*, 
                                        struct list_node_s*, struct list_node_s*);

  //function pointers
  bool (*extract_container)(struct list_node_s*, void*, size_t*, size_t);

} list_node_t;

//the entire list itself
typedef struct list_s
{
  list_node_t *head, *tail;
  u32int size; //number of elements

  //reserved pointer of any extensions
  // can be used by assigning a struct that extends this struct
  void *reserve;
  size_t reserve_sz;

  //internal list functions
  list_node_t *(*__get_node_at__)(struct list_s*, u32int);

  //function pointers
  bool (*push_front)(struct list_s*, void*, size_t);
  bool (*pop_front)(struct list_s*);
  bool (*push_back)(struct list_s*, void*, size_t);
  bool (*pop_back)(struct list_s*);
  bool (*at)(struct list_s*, u32int, void*, size_t*, size_t);
  bool (*insert)(struct list_s*, u32int, void*, size_t);
  bool (*clone)(struct list_s*, struct list_s*);
  bool (*free_all)(struct list_s*);
} list_t;

//List node function declarations

void init_list_node(list_t *master, list_node_t *node, list_node_t *prev, 
                    list_node_t *next, void *elem, size_t size);

list_node_t *__list_node_clone_node__(list_node_t *self, list_t *master, 
                                      list_node_t *prev, list_node_t *next);

bool list_node_extract_container(list_node_t *node, void *outData, size_t *outSize, size_t actualContainerSize);

//List node function declarations

//List function declarations

list_node_t *__list_get_node_at__(list_t *self, u32int index);

bool list_push_front(list_t *self, void *elem, size_t size);

bool list_push_back(list_t *self, void *elem, size_t size);

bool list_pop_front(list_t *self);

bool list_pop_back(list_t *self);

bool list_at(list_t *self, u32int index, void *outData, size_t *outSize, size_t actualContainerSize);

bool list_insert(list_t *self, u32int index, void *elem, size_t size);

bool list_clone(list_t *self, list_t *ret);

bool list_free_all(list_t *self);

void init_list_empty_head(list_t *list);

//List function declarations

#endif //LIST_IMPLEMENTATION_H
