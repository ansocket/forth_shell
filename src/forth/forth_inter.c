#include "forth.h"
#include "forth_dict.h"
#include "forth_inter.h"
#include "string.h"
static const char term[] = {' ','\t','\n','\r','\0'};

static int is_term(char ch)
{
    for(int i = 0; i < sizeof(term); i++)
    {
        if(ch == term[i]) return 1;
    }
    return 0;
}

char* forth_inter_token(char** buf)
{
    char* res = NULL;
    if(buf == NULL) return res;
    
    while(is_term(**buf))
    {
        (*buf)++;
        if(**buf == '\0')
            break;
    } 
    int len = strlen(*buf);
    if(len == 0) return res;
    res = *buf;
    while(len > 0)
    {
        if(is_term(**buf))
        {
            **buf = '\0';
            break;
        }
        len--;
        (*buf)++;
    }
    return res;
}