#include <stdio.h>
#include <stdlib.h>

#include "src/log_info/log_errors.h"
#include "src/generals_func/generals.h"

#include "src/word_reader/word_reader.h"
#include "src/list/list.h"


int main (int argc, char *argv[])
{  
    #ifdef USE_LOG
        if (OpenLogsFile()) 
            return OPEN_FILE_LOG_ERR;
    #endif

    if (argc != 2)
        return PROCESS_ERROR(EXIT_FAILURE, "incorrect number of parameters\n");

    char *input_file_name = argv[1];

    Text text = {};
    TextCtor(&text);    

    GetText(&text, input_file_name);

    List list = {};
    ListCtor(&list, Init_list_capacity);

    for (size_t it = 0; it < text.word_cnt; it++)
    {
        ListInsertBack(&list, text.words + it);
    }

    for (int it = 0; it < text.word_cnt; it++)
    {
        Word *word = ListGetVal(&list, it + 1);
        printf ("%s %lu\n", word->str, word->len);
    }

    printf ("%d", list.size_data);

    TextDtor(&text);

    #ifdef USE_LOG
        if (CloseLogsFile ())
            return OPEN_FILE_LOG_ERR;
    #endif

    return EXIT_SUCCESS;
}