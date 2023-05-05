#ifndef _WORD_READER_H_
#define _WORD_READER_H_

#include "../log_info/log_errors.h"
#include "../generals_func/generals.h"


const size_t Init_list_capacity = 200;


struct Word
{
    char *str  = nullptr;
    size_t len = 0;
};

struct Text
{
    char *buffer = nullptr;
    size_t file_size = 0;

    Word *words = nullptr;
    size_t word_cnt      = 0;

    int fdin    = 0;
};

enum Word_reader_errors
{
    TEXT_STR_CTOR_ERR   = -1,
    TEXT_STR_DTOR_ERR   = -2,

    GET_TEXT_ERR        = -3,

    PARSE_TEXT_ERR      = -4, 

    INSERT_WORD_ERR     = -5,
};

//====================================================================================================


int TextCtor    (Text *text_str);

int TextDtor    (Text *text_str);

int GetText     (Text *text_str, const char *file_name);


//====================================================================================================


#endif