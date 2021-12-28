#include <unistd.h>
#include <sys/types.h>

#include "helper_file.h"

void helper_log_posix_header(char* prestr, char* str, size_t n)
{
    unsigned int i = 0;
    char c;

    while(*prestr != '\0')
        write(STDOUT_FILENO, prestr++, 1);

    while(i < n)
    {
        if(str[i] == '\0')
            c = '.';
        else if(str[i] == ' ')
            c = '_';
        else
            c = str[i];
        
        write(STDOUT_FILENO, &c, 1);
        i++;
    }
    
    write(STDOUT_FILENO, "\n", 1);
}