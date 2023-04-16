#include "hash_functions.h"

//=========================================================================

size_t DumbHash (const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    return 1ull;
}

//=========================================================================

size_t FirstByteHash (const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    if (size == 0)
        return 0ull;
    
    return data[0];
}


//=========================================================================

size_t LenHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    return size;
}

//=========================================================================

size_t AsciiSumHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    size_t hash = 0;

    for (size_t it = 0; it < size; it++)
        hash += data[it];

    return hash;
}

//=========================================================================

size_t RolHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    const size_t Shift = sizeof(size_t) * 8 - 1;

    size_t hash = 0;

    for (size_t it = 0; it < size; it++)
    {
        hash = ((hash << 1) | (hash >> Shift));
        hash ^= data[it];

    }

    return hash;
}

//=========================================================================

size_t RorHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    const size_t Shift = sizeof(size_t) * 8 - 1;

    size_t hash = 0;

    for (size_t it = 0; it < size; it++)
    {
        hash = ((hash >> 1) | (hash << Shift));
        hash ^= data[it];

    }

    return hash;
}

//=========================================================================

size_t FAQ6Hash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    size_t hash = 0;

    for (size_t it = 0; it < size; it++)
    {
        hash += (uint8_t)(data[it]);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}



//=========================================================================
