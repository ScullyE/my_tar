#ifndef MY_STRING 
#define MY_STRING 

int my_strlen(char* str);
int my_strcmp(char* str1, char* str2);
char* my_strrev(char* str);
char* my_strcat(char* dest, const char* src);
char* my_strdup(char* str);
char* my_strstr(char* search_in, char* search_for);

char* my_strmemcpy(char* dest, const char* src, size_t n);
char* my_strcpy(char* dest, const char* src);
char* my_strncpy(char* dest, const char* src, size_t n);
char* my_strncpy_no_zero_filling(char* dest, const char* src, size_t n);
char* my_strncpy_n(char* dest, const char* src, size_t n, size_t* i);
char* append_n_to_string(char* dest, const char* src, size_t n);


char* my_usign_itoa_right_shift_fixed_length(int value, char* str, int base, int length);
unsigned int my_usign_atoi(char* str, int base);

#endif 