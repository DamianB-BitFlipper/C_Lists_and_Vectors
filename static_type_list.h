
#ifndef STATIC_TYPE_LIST_H
#define STATIC_TYPE_LIST_H

#include "utils.h"  //special types, ie) u32int, u8int
#include "dynamic_type_list.h"  //list definitions

/*********Static Type List macros*********/
// In naming: ST stands for 'Static Type'

//!!! ALL MACROS TAKE NON-POINTER ARGS !!!//

/*initializes a list*/
#define STATIC_TYPE_LIST_INIT(list, type) init_static_type_list_empty_head(&list, sizeof(type))

/*********Static Type List macros*********/

//List function declarations

void init_static_type_list_empty_head(list_t *list, size_t elem_sz);

bool static_type_list_insert(list_t *self, u32int index, void *elem, size_t size);

bool static_type_list_push_back(list_t *self, void *elem, size_t size);

bool static_type_list_push_front(list_t *self, void *elem, size_t size);

//List function declarations

typedef struct static_list_reserve_s
{
  const size_t element_sz;
} static_list_reserve_t;

#endif //STATIC_TYPE_LIST_H
