#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"


typedef uint32_t (*hash_func_t) (const char*, const size_t);


uint32_t    DumbHash         (const char *data, const size_t size);

uint32_t    FirstByteHash    (const char *data, const size_t size);

uint32_t    LenHash         (const char *data, const size_t size);

uint32_t    AsciiSumHash    (const char *data, const size_t size);

uint32_t    RolHash         (const char *data, const size_t size);

uint32_t    RorHash         (const char *data, const size_t size);

uint32_t    CRC32Hash       (const char *data, const size_t size);

extern "C" uint32_t NASMCRC32 (const char* data, const size_t size);


#endif