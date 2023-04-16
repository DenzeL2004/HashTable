#include "hash_table.h"

//=========================================================================

int HashTableCtor(Hash_table *hash_table, size_t capacity, hash_func_t hash_func)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");
    assert(hash_func  != nullptr && "hash function ptr is nullptr");

    hash_table->capacity = capacity;

    hash_table->data = (List*) calloc(capacity, sizeof(List));
    if (CheckNullptr(hash_table->data))
        return PROCESS_ERROR(HASH_TABLE_CTOR_ERR, "Memory allocation failed\n");

    for (size_t it = 0; it < capacity; it++)
    {
        if (ListCtor(hash_table->data + it, Init_list_capacity))
            return PROCESS_ERROR(HASH_TABLE_CTOR_ERR, "Ctor list by index: %lu failed\n", it);
    }

    hash_table->hash_func = hash_func;

    return 0;
}

//=========================================================================

int HashTableDtor(Hash_table *hash_table)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    for (size_t it = 0; it < hash_table->capacity; it++)
    {
        if (ListDtor(hash_table->data + it))
            return PROCESS_ERROR(HASH_TABLE_DTOR_ERR, "Dtor list by index: %lu failed\n", it);
    }

    hash_table->capacity  = 0;
    hash_table->hash_func = nullptr;

    return 0;
}

//=========================================================================

int HashTableInsert(Hash_table *hash_table, elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    size_t hash = (*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity;

    ListInsertBack(hash_table->data + hash, val);
    
    return 0;
}

//=========================================================================