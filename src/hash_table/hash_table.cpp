#include "hash_table.h"


//=========================================================================

int HashTableCtor(Hash_table *hash_table, size_t capacity, hash_func_t hash_func)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");
    assert(hash_func  != nullptr && "hash function ptr is nullptr");

    hash_table->capacity = capacity;

    hash_table->buckets = (List*) calloc(capacity, sizeof(List));
    if (CheckNullptr(hash_table->buckets))
        return PROCESS_ERROR(HASH_TABLE_CTOR_ERR, "Memory allocation failed\n");

    for (size_t it = 0; it < capacity; it++)
    {
        if (ListCtor(hash_table->buckets + it, Init_list_capacity))
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
        if (ListDtor(hash_table->buckets + it))
            return PROCESS_ERROR(HASH_TABLE_DTOR_ERR, "Dtor list by index: %lu failed\n", it);
    }

    free(hash_table->buckets);

    hash_table->capacity  = 0;
    hash_table->hash_func = nullptr;

    return 0;
}

//=========================================================================

int HashTableInsert(Hash_table *hash_table, const elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    uint32_t hash =  (uint32_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    ListInsertBack(hash_table->buckets + hash, val);
    
    return 0;
}

//=========================================================================

size_t HashTableFind(Hash_table *hash_table, const elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    hash_t hash = (hash_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    return ListFindVal(&hash_table->buckets[hash], val);
}

//TODO inline:
//измерить по каждой функции по отдельности
//время по конкретному фремени

//=========================================================================


int HashTableErase(Hash_table *hash_table, const elem_t val)
{
    assert(hash_table != nullptr && "hash table ptr is nullptr");

    size_t ind = HashTableFind(hash_table, val);
    if (ind == Invalid_ind)
    {
        LOG_REPORT("No item in table\n");
        return 0;
    }

    uint32_t hash =  (uint32_t)((*(hash_table->hash_func))(val->str, val->len) % hash_table->capacity);

    ListErase(&(hash_table->buckets[hash]), ind);

    return 0;
}

//=========================================================================