#ifndef _LIST_H_
#define _LIST_H_

#include "config_list.h"
#include "../log_info/log_def.h"
#include "../generals_func/generals.h"

const size_t Identifier_free_node = -1;

const elem_t Dummy_element = nullptr;

struct Node
{
    elem_t val = 0;
    size_t next = 0;
    size_t prev = 0;
};

struct List
{
    size_t capacity       = 0;
    size_t size_data      = 0; 
    size_t cnt_free_nodes = 0;

    Node *data = nullptr;

    size_t head_ptr  = 0;
    size_t tail_ptr  = 0;
    size_t free_ptr  = 0;

    int is_linearized = 0; 
};


enum List_func_err
{
    LIST_CTOR_ERR           = -1,
    LIST_DTOR_ERR           = -2,

    LIST_INVALID_ERR        = -3,

    PTR_OUT_OF_MEMORY_ERR   = -6,
    
    LIST_INSERT_ERR         = -7,
    LIST_ERASE_ERR          = -8,

    DATA_INIT_ERR           = -9,
    DATA_CLEAR_ERR          = -10,

    GET_LOGICAL_PTR_ERR     = -11,
    GET_VAL_ERR             = -12,

    CHECK_IND_ERR           = -13,

    LIST_RESIZE_ERR         = -14,    
    LIST_RECALLOC_ERR       = -15,

    LIST_LINEARIZE_ERR      = -16,
    
    LIST_DRAW_GRAPH_ERR     = -17,
    LIST_GET_VAL_ERR        = -18,

    LIST_FIND_VAL_ERR       = -19,
    LIST_CONTAIN_VAL_ERR    = -20,
};

enum List_err
{
    CAPACITY_LOWER_SIZE         = (1 << 2),

    ILLIQUID_FREE_PTR           = (1 << 3),

    DATA_IS_NULLPTR             = (1 << 4),

    INCORRECT_LINEARIZED        = (1 << 5),

    DATA_NODE_INCORRECT         = (1 << 6),
    DATA_FREE_NODE_INCORRECT    = (1 << 7),

    ILLIQUID_HEAD_PTR           = (1 << 8),
    ILLIQUID_TAIL_PTR           = (1 << 9),
    
};



int ListCtor (List *list, const size_t capacity);

int ListDtor (List *list);


/** 
 * @brief Adds a node to the list
 * @version 1.0.0
 * @param [in] *list Structure List pointer
 * @param [in] ind The index of the memory location before which we want to add a new node. (The node at the given index must be initialized)
 * @param [in] val The value of the added node
 * @return If a vertex has been added, it returns the Physical Pointer where the element is located, otherwise a negative number
*/
size_t ListInsertPrevInd (List *list, const size_t ind, const elem_t val);

size_t ListInsertFront     (List *list, const elem_t val);

size_t ListInsertBack      (List *list, const elem_t val);


/** 
 * @brief Removes a node by its index
 * @version 1.0.0
 * @param [in] *list Structure List pointer
 * @param [in] ind The pointer by which we will delete the node. (The node at the given index must be initialized)
 * @return Returns zero if the node is deleted, otherwise returns a non-zero number
*/
int ListErase (List *list, const size_t ind);


size_t GetLogicalIndex (const List *list, const size_t ind);


/**
 * @brief Get value by physical index
 * @param [in] *list Structure List pointer
 * @param [in] ind The physical index by which we will get the node. (The node at the given index must be initialized)
 * @return Returns a poison value if an element referencing error has occurred, otherwise the return value is assumed to be the actual value of the node
*/
elem_t ListGetVal    (const List *list, const size_t ind);


size_t ListFindVal   (const List *list, const elem_t val);

/** 
 * @brief Change value by physical index
 * @version 1.0.0
 * @param [in] *list Structure List pointer
 * @param [in] ind The pointer by which we will chage the node. (The node at the given index must be initialized)
 * @return Returns zero if the node is change, otherwise returns a non-zero number
*/
int ListChangeVal (const List *list, const size_t ind, const elem_t val);

int ListLinearize (List *list);

#define ListDump(list, ...)                       \
        ListDump_ (list, LOG_ARGS, __VA_ARGS__)

int ListDump_ (const List *list,
                const char* file_name, const char* func_name, int line, const char *format, ...);


extern "C" int NASMGetLogicalIndex(const List *list, const size_t ind);

//elem_t ASMListGetVal (const List *list, const size_t ind);

extern "C" elem_t NASMListGetVal (const List *list, const size_t ind);

#endif  //#endif _LIST_H_