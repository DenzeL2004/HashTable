#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"

typedef uint32_t hash_t;

typedef hash_t (*hash_func_t) (const char*, const size_t);


hash_t      DumbHash        (const char *data, const size_t size);
    
hash_t      FirstByteHash   (const char *data, const size_t size);
    
hash_t      LenHash         (const char *data, const size_t size);
    
hash_t      AsciiSumHash    (const char *data, const size_t size);
    
hash_t      RolHash         (const char *data, const size_t size);
    
hash_t      RorHash         (const char *data, const size_t size);

hash_t      CRC32Hash       (const char *data, const size_t size);

#endif