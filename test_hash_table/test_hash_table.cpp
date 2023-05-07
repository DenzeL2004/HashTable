#include "../src/word_reader/word_reader.h"
#include "../src/hash_table/hash_table.h"

//#include "../src/hash_table/hash_functions.h"

#include "test_hash_table.h"

#include "config.h"

//============================================================================================

static int          TestDistribution    (const Text *text);

static int          TestFind            (const Text *text);

static void         StartFind           (Hash_table *hash_table, const Text *text);

inline static void  LoadData            (Hash_table *hash_table, const Text *text);

//============================================================================================

int ExeTest (const char *input_file_name)
{
    
    assert(input_file_name != nullptr && "input file name is nullptr");

    Text text = {};
    
    if (TextCtor(&text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextCtor failed\n");

    if (GetText(&text, input_file_name))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "file: \'%s\' read failed, no tests run\n", input_file_name);

    #ifdef TEST_DISTRIBUTION

    TestDistribution(&text);

    #endif

    #ifdef TEST_FIND

    TestFind(&text);

    #endif

    if (TextDtor(&text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextDtor failed\n");

    return 0;
}

//======================================================================================================

static int TestDistribution(const Text *text)
{
    assert(text  != nullptr && "text is nullptr");

    FILE *fpout = OpenFilePtr(Name_result_file, "w");
    if (CheckNullptr(fpout))
        PROCESS_ERROR(EXECUTE_TEST_ERR, "open file: \'%s\' to wite failed\n.", Name_result_file);

    for (size_t hash_func_num = 0; hash_func_num < Count_hash_func; hash_func_num++)
    {
        fprintf(fpout,  "\n%s%c ", Hash_function_name[hash_func_num], Separate_symbol);

        Hash_table hash_table = {};
        if (HashTableCtor(&hash_table, Hash_table_capacity, Hash_function_ptr[hash_func_num]))
            return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableCtor failed.\n");

        LoadData(&hash_table, text);

        for (size_t it = 0; it < hash_table.capacity; it += Step_print)
        {
            fprintf(fpout, "%lu", hash_table.buckets[it].size_data);
            if(it + Step_print < Hash_table_capacity)
                fprintf(fpout, "%c ", Separate_symbol);
        }

        if (HashTableDtor(&hash_table))
            return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableDtor failed.\n");
        
        fprintf(fpout, "\n\n");
    }

    if (CloseFilePtr(fpout))
        PROCESS_ERROR(EXECUTE_TEST_ERR, "|%p| failed", (char*)fpout);

    return 0;
}

//======================================================================================================

static int TestFind(const Text *text)
{
    assert(text  != nullptr && "text is nullptr");

    Hash_table hash_table = {};
    if (HashTableCtor(&hash_table, Hash_table_capacity, (hash_func_t)FastCRC32Hash))
        return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableCtor failed.\n");

    LoadData(&hash_table, text);

    Text test_text = {};
    
    if (TextCtor(&test_text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextCtor failed\n");

    if (GetText(&test_text, Test_input_file))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "file: \'%s\' read failed, no tests run\n", Test_input_file);

    StartFind(&hash_table, &test_text);

    if (TextDtor(&test_text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextDtor failed\n");

    if (HashTableDtor(&hash_table))
        return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableDtor failed.\n");

    return 0;
}

static void StartFind(Hash_table *hash_table, const Text *text)
{
    assert(hash_table != nullptr && "hash table is nullptr");
    assert(text != nullptr && "text is nullptr");

    for (size_t it = 0; it < Count_query; it++)
    {
        size_t ind = HashTableFind(hash_table, &(text->words[it]));

        hash_t hash = hash_table->hash_func(text->words[it].str, text->words[it].len) % hash_table->capacity;
        printf ("!!%s %s\n", text->words[it].str, hash_table->buckets[hash].data[ind].val->str);   
    }

    return;
}

//======================================================================================================

inline static void LoadData(Hash_table *hash_table, const Text *text)
{
    assert(hash_table != nullptr && "hash table is nullptr");
    assert(text != nullptr && "text is nullptr");

    for (size_t it = 0; it < text->word_cnt; it++)
    {
        HashTableInsert(hash_table, text->words + it);
    }

    return;
} 

//======================================================================================================