#include <stdio.h>
#include <stdlib.h>

#include "test_hash_table/test_hash_table.h"


int main (int argc, char *argv[])
{  
    #ifdef USE_LOG
        if (OpenLogsFile()) 
            return OPEN_FILE_LOG_ERR;
    #endif

    if (argc != 2)
        return PROCESS_ERROR(EXIT_FAILURE, "incorrect number of parameters\n");

    char *input_file_name = argv[1];

    if (ExeTest(input_file_name))
        return PROCESS_ERROR(EXIT_FAILURE, "execute file failed\n");

    #ifdef USE_LOG
        if (CloseLogsFile ())
            return OPEN_FILE_LOG_ERR;
    #endif

    return EXIT_SUCCESS;
}