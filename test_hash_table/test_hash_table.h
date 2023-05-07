#ifndef _TEST_HASH_TABLE_H_
#define _TEST_HASH_TABLE_H_


#include "../src/generals_func/generals.h"
#include "../src/log_info/log_errors.h"

#include "../src/hash_table/hash_table.h"
#include "../src/hash_table/hash_functions.h"

static const char* Hash_function_name[] = 
{
    "DumbHash",        
    "FirstByteHash",     
    "LenHash",         
    "AsciiSumHash",    
    "RolHash",    
    "RorHash",  
    "CRC32Hash",
    "FastCRC32Hash"         
};

const hash_func_t Hash_function_ptr[] = 
{
    DumbHash,         
    FirstByteHash,   
    LenHash,         
    AsciiSumHash,    
    RolHash,   
    RorHash,  
    CRC32Hash,
    FastCRC32Hash       
};

const size_t Count_hash_func = sizeof(Hash_function_name) / sizeof(Hash_function_name[0]);

enum Test_hash_table_err
{

    EXECUTE_TEST_ERR        = -1,

    DISTRIBUTION_TEST_ERR   = -2,

    FIND_TEST_ERR           = -3,
};

//=============================================================================================

int ExeTest (const char *input_file_name);

#endif