#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"

typedef uint32_t hash_t;

typedef char* hash_key_t;

typedef hash_t (*hash_func_t) (const hash_key_t);

hash_t      DumbHash        (const hash_key_t key);
    
hash_t      FirstByteHash   (const hash_key_t key);
    
hash_t      LenHash         (const hash_key_t key);
    
hash_t      AsciiSumHash    (const hash_key_t key);
    
hash_t      RolHash         (const hash_key_t key);
    
hash_t      RorHash         (const hash_key_t key);

hash_t      CRC32Hash       (const hash_key_t key);


hash_t      FastCRC32Hash   (const hash_key_t key);

#endif