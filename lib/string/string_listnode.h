#ifndef STRUCT_STRING_LISTNODE 
#define STRUCT_STRING_LISTNODE 

typedef struct s_string_listnode 
{ 
    char* value; 
    struct s_string_listnode* next; 
} string_listnode; 


string_listnode* string_listnode_create_element(char* str);
string_listnode* string_listnode_push_element(string_listnode* head, char* str);
void switch_string_listnodes(string_listnode* sln1, string_listnode* sln2);
void string_listnode_print(string_listnode* head, char* sep, unsigned int print_at_end);
void string_listnode_free(string_listnode* head);


#endif 