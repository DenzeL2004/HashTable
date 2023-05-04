#include "hash_table.h"


//=========================================================================

int HashTableCtor(Hash_table *hash_table, size_t capacity, hash_func_t hash_func)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");
    assert(hash_func  != nullptr && "hash function ptr is nullptr");

    hash_table->capacity = capacity;

    hash_table->containers = (List*) calloc(capacity, sizeof(List));
    if (CheckNullptr(hash_table->containers))
        return PROCESS_ERROR(HASH_TABLE_CTOR_ERR, "Memory allocation failed\n");

    for (size_t it = 0; it < capacity; it++)
    {
        if (ListCtor(hash_table->containers + it, Init_list_capacity))
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
        if (ListDtor(hash_table->containers + it))
            return PROCESS_ERROR(HASH_TABLE_DTOR_ERR, "Dtor list by index: %lu failed\n", it);
    }

    free(hash_table->containers);

    hash_table->capacity  = 0;
    hash_table->hash_func = nullptr;

    return 0;
}

//=========================================================================

int HashTableInsert(Hash_table *hash_table, elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    uint32_t hash =  (uint32_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    ListInsertBack(hash_table->containers + hash, val);
    
    return 0;
}

//=========================================================================

long HashTableFind(Hash_table *hash_table, elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    uint32_t hash = (uint32_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    long list_size = hash_table->containers[hash].size_data;
    size_t val_len = val->len;

    for (int it = 1; it <= list_size; it++)
    {
        int logical_ind = NASMGetLogicalIndex(&(hash_table->containers[hash]), it);         //::OPTIMIZE
        elem_t cur_val  = NASMListGetVal(&(hash_table->containers[hash]), logical_ind);     //::OPTIMIZE

        if (cur_val->len == val_len)
        {
            if (FastStrncmp(cur_val->str, val->str, val_len))                                //::OPTIMIZE
            {
                return it;
            }
        }
    }

    return Invalid_ind;
}

//=========================================================================


int HashTableErase(Hash_table *hash_table, elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    int ind = HashTableFind(hash_table, val);
    if (ind == Invalid_ind)
    {
        LOG_REPORT("No item in table\n");
        return 0;
    }

    uint32_t hash =  (uint32_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    ListErase(&(hash_table->containers[hash]), ind);

    return 0;
}

//=========================================================================