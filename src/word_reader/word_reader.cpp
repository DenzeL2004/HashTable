#include <assert.h>
#include <string.h>

#include "word_reader.h"



static int      ReadFromFile     (Text *text_str, int fdin);

static int      ParseText        (Text *text_str);

static size_t   GetCountWord     (Text *text_str);


//====================================================================================================

int GetText(Text *text_str, const char *file_name)
{
    assert(text_str  != nullptr && "text struct is nullptr");
    assert(file_name != nullptr && "file name is nullptr");

    int fdin = OpenFileDescriptor(file_name, O_RDONLY);
    if (fdin <= 0)
        PROCESS_ERROR(GET_TEXT_ERR, "Open file \'%s\' failed. Descriptor: %d", file_name, fdin);
        
    if (ReadFromFile(text_str, fdin))
        return PROCESS_ERROR(GET_TEXT_ERR, "Read from file failed\n");
    
    if (CloseFileDescriptor(fdin))
        PROCESS_ERROR(GET_TEXT_ERR, "Close file descriptor: %d failed", fdin);
       
    if (ParseText(text_str))
        return PROCESS_ERROR(TEXT_STR_CTOR_ERR, "parse input file failed\n");
    
    return 0;
}

//====================================================================================================

int TextCtor(Text *text_str)
{
    assert(text_str  != nullptr && "text struct is nullptr");

    
    text_str->buffer    = nullptr;
    text_str->file_size = 0;

    text_str->words     = nullptr;
    text_str->word_cnt  = 0; 

    return 0;
}


//====================================================================================================

int TextDtor(Text *text_str)
{
    assert(text_str != nullptr && "text struct is nullptr");

    free(text_str->words);
    text_str->word_cnt  = 0; 

    free(text_str->buffer);
    text_str->file_size = 0;


    return 0;
}

//====================================================================================================

static int ReadFromFile(Text *text_str, int fdin)
{
    assert(text_str  != nullptr && "text struct is nullptr");
    assert(fdin > 0 && "file descriptor failed");

    struct stat file_info = {};
    fstat (fdin, &file_info);

    text_str->file_size = file_info.st_size;

    text_str->buffer = (char*)calloc(text_str->file_size, sizeof(char));
    if (CheckNullptr(text_str->buffer))
        return PROCESS_ERROR(TEXT_STR_CTOR_ERR, "memory allocation failed\n");
       

    size_t read_cnt = read(fdin, text_str->buffer, text_str->file_size);
    if (read_cnt != text_str->file_size)
        return PROCESS_ERROR(TEXT_STR_CTOR_ERR, "incorrect reading from file. Read: %lu, must read: %lu\n", 
                                                                        read_cnt, text_str->file_size);
        

    return 0;
} 

//====================================================================================================

static int ParseText(Text *text_str)
{
    assert(text_str != nullptr && "text struct pointer is nullptr");

    text_str->word_cnt = GetCountWord(text_str);

    text_str->words = (Word*) calloc(text_str->word_cnt + 1, sizeof(Word));
    if (CheckNullptr(text_str->words))
        return PROCESS_ERROR(PARSE_TEXT_ERR, "memory allocation for array of words failed\n");

    size_t char_it = 0;
    size_t word_it = 0;

    size_t file_size = text_str->file_size;

    while (char_it < file_size)
    {
        if (isalpha(text_str->buffer[char_it]))
        {
            size_t start_pos = char_it;
            while (char_it < file_size && isalpha(text_str->buffer[char_it]))
            {
                char_it++;
            }

            text_str->words[word_it].str = (text_str->buffer + start_pos);
            text_str->words[word_it].len = char_it - start_pos; 

            word_it++;  
        }
        
        char_it++;
    }
    
    return 0;
}

//====================================================================================================


static size_t GetCountWord(Text *text_str)
{
    assert(text_str != nullptr && "text struct pointer is nullptr");

    size_t word_cnt = 0;
    size_t char_it  = 0;

    size_t file_siz = text_str->file_size;

    while (char_it < file_siz)
    {
        if (isalpha(text_str->buffer[char_it]))
        {
            word_cnt++;
            while (char_it < file_siz && isalpha(text_str->buffer[char_it]))
            {
                char_it++;
            }

            if (char_it < file_siz)
                text_str->buffer[char_it] = '\0';   
        }
        
        char_it++;
    }
    
    return word_cnt;
}

//====================================================================================================
