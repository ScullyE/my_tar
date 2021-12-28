#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "my_string.h"


int my_strcmp(char* str1, char* str2)
{
    while(*str1 != '\0' && *str2 != '\0')
    {
        if(*str1 > *str2 || *str1 < *str2)
            return *str1 - *str2;
        
        str1++;
        str2++;
    }
    return *str1 - *str2;
}


char* my_strcpy(char* dest, const char* src)
{
    char* ret = dest;
    while(*src != '\0')
        *dest++ = *src++;
    
    *dest = '\0';
    return ret;
}


char* my_strncpy(char* dest, const char* src, size_t n)
{
    long unsigned int count = 0;
    char* ret = dest;

    while(*src != '\0' && count < n)
    {
        *dest++ = *src++;
        count++;
    }

    while(count < n)
    {
        *dest++ = '\0';
        count++;
    }
    
    return ret;
}


char* my_strmemcpy(char* dest, const char* src, size_t n)
{
    long unsigned int count = 0;
    char* ret = dest;

    while(count < n)
    {
        dest[count] = src[count];
        count++;
    }
    
    return ret;
}


char* my_strncpy_no_zero_filling(char* dest, const char* src, size_t n)
{
    long unsigned int count = 0;
    char* ret = dest;

    while(count < n)
    {
        *dest++ = *src++;
        count++;
    }
    
    return ret;
}


char* my_strncpy_n(char* dest, const char* src, size_t n, size_t* i)
{
    long unsigned int count = 0;
    char* ret = dest;

    while(*src != '\0')
    {
        *dest++ = *src++;
        count++;
    }

    while(count < n)
    {
        *dest++ = '\0';
        count++;
    }
    
    *i += n;

    return ret;
}


int my_strlen(char* str)
{
    int count = 0;

    while(*str++ != '\0')
        count++;

    return count;
}


char* my_strrev(char* str)
{
    int length = my_strlen(str);
    char temp;

    for(int i = 0; i < length / 2; i++)
    {
        temp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = temp;
    }

    return str;
}


char* my_strdup(char* str)
{
    char* dup = malloc((my_strlen(str) + 1) * sizeof(char));
    char* ret = dup;

    while(*str != '\0')
        *dup++ = *str++;

    *dup = '\0';
    return ret;
}


char* my_strstr(char* search_in, char* search_for)
{
    int i = 0;
    char* ret = "NULL";

    while(*search_in != '\0')
    {
        if(*search_in == *search_for)
        {
            i = 1;
            while(search_in[i] == search_for[i] && search_for[i] !='\0')
                i++;

            if(search_for[i] == '\0' && search_in[i - 1] == search_for[i - 1])
                return search_in;
        }
        search_in++;        
    }
    return ret;
}


char* my_strcat(char *dest, const char *src)
{
    char* ret = dest;

    while(*dest != '\0') 
        dest++;

    while(*src != '\0')
        *dest++ = *src++;

    *dest = '\0';

    return ret;
}


char* append_n_to_string(char *dest, const char *src, size_t n)
{
    long unsigned int count = 0;

    while(count < n)
    {
        *dest++ = *src++;
        count++;
    }

    return dest;
}


char* my_usign_itoa_right_shift_fixed_length(int value, char* str, int base, int length)
{
    int remain;
    char* ret = str;

    str[length - 1] = '\0';
    for(int i = length - 2; i >= 0; i--)
    {
        remain = value % base;
        str[i] = (remain > 9) ? remain + 'A' - 10: remain + '0';
        value /= base;
    }

    return ret;
}


unsigned int my_usign_atoi(char* str, int base)
{
    unsigned int res = 0;

    while(*str != '\0')
    {
        if(*str >= '0' && *str <= '9')
            res = res * base + *str - '0';

        else if(*str >= 'a' && *str <= 'f')
            res = res * base + *str - 'a';
            
        else if(*str >= 'A' && *str <= 'F')
            res = res * base + *str - 'A';
        else
            return res;
        str++;
    }

    return res;
}