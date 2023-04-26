#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"

#include "../list/list.h"
#include "hash_functions.h"


struct Hash_table
{
    size_t capacity = 0;

    List *containers = nullptr;

    hash_func_t hash_func = nullptr;
};

enum Hash_table_err
{
    HASH_TABLE_CTOR_ERR     = -1,
    HASH_TABLE_DTOR_ERR     = -2,

    HASH_TABLE_INSERT_ERR   = -3,
};

//=======================================================================================

int     HashTableCtor       (Hash_table *hash_table, size_t capacity, hash_func_t hash_func);

int     HashTableDtor       (Hash_table *hash_table);

int     HashTableInsert     (Hash_table *hash_table, elem_t val);


long    HashTableFind       (Hash_table *hash_table, elem_t val);


int     HashTableErase      (Hash_table *hash_table, elem_t val);

//=======================================================================================

#endif