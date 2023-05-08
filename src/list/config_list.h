#ifndef _LIST_CONFIG_H_
#define _LIST_CONFIG_H_

#include <stdint.h>
#include "../word_reader/word_reader.h"

typedef char *elem_t;


const elem_t Poison_val  = nullptr;  //<- In free nodes is written

const size_t Poison_ptr  = -126;   //<- Written to a pointer when the list is cleared

const size_t Invalid_ind = 0;


#define USE_LOG                 //<- connect when we use logs

//#define CHECK_LIST 

#define LIST_DATA_CHECK         //<- Checking non-free list nodes for correct transitions and values

//#define CHECK_LIST

//#define GRAPH_DUMP

#define ELEM_T_SPEC "p"         //<- specifier character to print elem


//#define OPTIMIZE_FIND_VER


#endif  //endif _LIST_CONFIG_H_