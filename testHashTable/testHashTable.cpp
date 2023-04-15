
#include "testHashTable.h"
#include "../src/word_reader/word_reader.h"


int TestDistribution (int fdout, Text *text, hash_func_t *hash_func)
{
    assert(fdout >= 0 && "fdout is not positive number");
    assert(text       != nullptr && "text is nullptr");
    assert(hash_func  != nullptr && "hash function is nullptr");
    
    for (size_t it = 0; it < text->word_cnt; it++)
    {
        
    }

}