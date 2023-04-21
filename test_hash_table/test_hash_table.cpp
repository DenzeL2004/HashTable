#include "../src/word_reader/word_reader.h"
#include "../src/hash_table/hash_table.h"

#include "../src/hash_table/hash_functions.h"

#include "test_hash_table.h"

#include "config.h"

//============================================================================================

static int TestDistribution (FILE *fpout, Text *text, hash_func_t hash_func);

int ExeTest (const char *input_file_name)
{
    
    assert(input_file_name != nullptr && "input file name is nullptr");

    Text text = {};
    
    if (TextCtor(&text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextCtor failed\n");

    if (GetText(&text, input_file_name))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "file: \'%s\' read failed, no tests run\n", input_file_name);


    FILE *fpout = OpenFilePtr(Name_result_file, "w");
    if (CheckNullptr(fpout))
        PROCESS_ERROR(EXECUTE_TEST_ERR, "open file: \'%s\' to wite failed\n.", Name_result_file);

    fprintf (fpout, "\n");

    for (size_t it = 0; it < Count_hash_func; it++)
    {
        fprintf(fpout,  "\n%s%c ", Hash_function_name[it], Separate_symbol);

        TestDistribution(fpout, &text, Hash_function_ptr[it]);
        fprintf(fpout, "\n\n");
    }

    if (CloseFilePtr(fpout))
        PROCESS_ERROR(EXECUTE_TEST_ERR, "%p| failed", (char*)fpout);

    if (TextDtor(&text))
        return PROCESS_ERROR(EXECUTE_TEST_ERR, "TextDtor failed\n");

    return 0;
}

//======================================================================================================

static int TestDistribution (FILE *fpout, Text *text, hash_func_t hash_func)
{
    assert(fpout      != nullptr && "fpout is nullptr");
    assert(text       != nullptr && "text is nullptr");
    assert(hash_func  != nullptr && "haszh table is nullptr");

    Hash_table hash_table = {};
    if (HashTableCtor(&hash_table, Hash_table_capacity, hash_func))
        return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableCtor failed.\n");

    for (size_t it = 0; it < text->word_cnt; it++)
    {
        HashTableInsert(&hash_table, text->words + it);
    }

    for (size_t it = 0; it < hash_table.capacity; it += Step_print)
    {
        fprintf(fpout, "%ld", hash_table.data[it].size_data);
        if(it + Step_print < Hash_table_capacity)
            fprintf(fpout, "%c ", Separate_symbol);
    }

    if (HashTableDtor(&hash_table))
        return PROCESS_ERROR(DISTRIBUTION_TEST_ERR, "HashTableDtor failed.\n");

    return 0;
}

//======================================================================================================