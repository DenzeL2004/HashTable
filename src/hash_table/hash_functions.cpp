#include "hash_functions.h"

//=========================================================================

uint32_t DumbHash (const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    return 0u;
}

//=========================================================================

uint32_t FirstByteHash (const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    if (size == 0)
        return 0u;
    
    return data[0];
}


//=========================================================================

uint32_t LenHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    return (uint32_t)size;
}

//=========================================================================

uint32_t AsciiSumHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    uint32_t hash = 0u;

    for (size_t it = 0; it < size; it++)
        hash += data[it];

    return hash;
}

//=========================================================================

uint32_t RolHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    const uint32_t Shift = sizeof(uint32_t) * 8 - 1;

    uint32_t hash = 0u;

    for (size_t it = 0; it < size; it++)
    {
        hash = ((hash << 1) | (hash >> Shift));
        hash ^= data[it];

    }

    return hash;
}

//=========================================================================

uint32_t RorHash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    const uint32_t Shift = sizeof(uint32_t) * 8 - 1;

    uint32_t hash = 0u;

    for (size_t it = 0; it < size; it++)
    {
        hash = ((hash >> 1) | (hash << Shift));
        hash ^= data[it];
    }

    return hash;
}

//=========================================================================

uint32_t CRC32Hash(const char *data, const size_t size)
{
    assert(data != nullptr && "data pointer is nullptr");

    const uint32_t Init_val = 0xFFFFFFFFu; 

    uint32_t hash = Init_val;

    for (size_t it = 0; it < size; it++)
    {
        hash = (hash >> 8) ^ CRC32Table[(hash ^ data[it]) & 0xFF];
    }

    return hash ^ Init_val;
}

//=========================================================================
