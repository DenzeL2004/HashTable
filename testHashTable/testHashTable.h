#ifndef _TEST_HASH_TABLE_
#define _TEST_HASH_TABLE_

#include "../src/generals_func/generals.h"
#include "../src/log_info/log_errors.h"

#include "../src/hash_table/hash_table.h"

const char* hash_function_name[] = 
{
    "DumbHash",        
    "FirstByteHash",    
    "FirstByteHash"    
    "LenHash",         
    "AsciiSumHash",    
    "LeftShiftHash",    
    "RightShiftHash",  
    "FAQ6Hash"        
};

const hash_func_t* hash_function_ptr[] = 
{
    (hash_func_t*) DumbHash,         
    (hash_func_t*) FirstByteHash,    
    (hash_func_t*) FirstByteHash,    
    (hash_func_t*) LenHash,         
    (hash_func_t*) AsciiSumHash,    
    (hash_func_t*) LeftShiftHash,   
    (hash_func_t*) RightShiftHash,  
    (hash_func_t*) FAQ6Hash        
};

const size_t Count_hash = sizeof(hash_function_ptr) / sizeof(hash_function_ptr[0]);

#endif