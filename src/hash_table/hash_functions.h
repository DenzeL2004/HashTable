#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"


typedef size_t hash_func_t (const char *data, const size_t size);


size_t DumbHash         (const char *data, const size_t size);

size_t FirstByteHash    (const char *data, const size_t size);

size_t FirstByteHash    (const char *data, const size_t size);

size_t  LenHash         (const char *data, const size_t size);

size_t  AsciiSumHash    (const char *data, const size_t size);

size_t  LeftShiftHash   (const char *data, const size_t size);

size_t  RightShiftHash  (const char *data, const size_t size);

size_t  FAQ6Hash        (const char *data, const size_t size);

#endif