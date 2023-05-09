#include <assert.h>
#include <string.h>

#include "word_reader.h"



static int      ReadFromFile    (Text *text_str);

static int      ParseText       (Text *text_str);

static size_t   GetCountWord    (Text *text_str);


static char     IsAlphaNASM     (const char letter);


static inline int IsAlpha       (const char letter);


//====================================================================================================

int GetText(Text *text_str, const char *file_name)
{
    assert(text_str  != nullptr && "text struct is nullptr");
    assert(file_name != nullptr && "file name is nullptr");

    if (text_str->fdin != 0)
    {
        if (TextDtor(text_str))
            return PROCESS_ERROR(GET_TEXT_ERR, "text dtor in \'GetText\' failed");

        if (TextCtor(text_str))
            return PROCESS_ERROR(GET_TEXT_ERR, "text ctor in \'GetText\' failed");
    }

    text_str->fdin = OpenFileDescriptor(file_name, O_RDWR);
    if (text_str->fdin <= 0)
    {
        PROCESS_ERROR (ERR_FILE_OPEN, "open file descriptor opening failed\n");
        return GET_TEXT_ERR;
    }  


    if (ReadFromFile(text_str))
        return PROCESS_ERROR (GET_TEXT_ERR, "read file failed\n", text_str->fdin);
    
    if (ParseText(text_str))
        return PROCESS_ERROR (GET_TEXT_ERR, "parse text failed\n", text_str->fdin);   

    if (CloseFileDescriptor(text_str->fdin))
        return PROCESS_ERROR (GET_TEXT_ERR, "close file descriptor: %d failed\n", text_str->fdin);

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

    text_str->fdin      = 0;

    return 0;
}


//====================================================================================================

int TextDtor(Text *text_str)
{
    assert(text_str != nullptr && "text struct is nullptr");

    free(text_str->words);
    text_str->word_cnt  = 0; 

    free (text_str->buffer);
    text_str->file_size = 0;

    text_str->fdin      = 0;

    return 0;
}

//====================================================================================================

static int ReadFromFile(Text *text_str)
{
    assert(text_str  != nullptr && "text struct is nullptr");

    struct stat file_info = {};
    fstat(text_str->fdin, &file_info);

    text_str->file_size = file_info.st_size;

    text_str->buffer =  CreateVirtualBuf(text_str->fdin, PROT_READ, 0);
    if (CheckNullptr(text_str->buffer))
        return PROCESS_ERROR (GET_TEXT_ERR, "create virtual buffer failed\n");


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

    char *aligned_buffer = (char*)CreateAlignedBuffer(32, (text_str->word_cnt + 1) * 32);

    size_t char_it = 0;
    size_t word_it = 0;

    size_t file_size = text_str->file_size;

    while (char_it < file_size)
    {
        if (IsAlpha(text_str->buffer[char_it]))
        {
            size_t aligned_it = word_it * 32;
            while (char_it < file_size && IsAlpha(text_str->buffer[char_it]))
            {
                aligned_buffer[aligned_it] = text_str->buffer[char_it];
                aligned_it++;
                char_it++;
            }

            text_str->words[word_it].str = (aligned_buffer + word_it * 32);
            text_str->words[word_it].len = aligned_it - (word_it * 32); 

            word_it++;  
        }
        
        char_it++;
    }

    if (FreeVirtualBuf(text_str->fdin, text_str->buffer))
        return PROCESS_ERROR (TEXT_STR_DTOR_ERR, "free virtual buffer failed\n");
    text_str->buffer = aligned_buffer;
    
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
        if (IsAlpha(text_str->buffer[char_it]))
        {
            word_cnt++;
            while (char_it < file_siz && IsAlpha(text_str->buffer[char_it]))
            {
                char_it++;
            }
        }
        
        char_it++;
    }
    
    return word_cnt;
}

//====================================================================================================

static char IsAlphaNASM(const char letter)
{
    char ans = 0;
    __asm__ (
        ".intel_syntax noprefix\n\t"
        "and     %1, -33\n\t"
        "sub     %1, 65\n\t"
        "cmp     %1, 25\n\t"
        "setbe   %0\n\t"
        
        : "=r"(ans)
        : "r"(letter)
    );

    return ans;
}

static inline int IsAlpha(const char letter)
{
    return ((letter >= 'a') && (letter <= 'z')) || ((letter >= 'A') && (letter <= 'Z'));
}


