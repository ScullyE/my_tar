#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "string_listnode.h"
#include "my_string.h"


string_listnode* string_listnode_create_element(char* str)
{
    string_listnode* new = malloc(sizeof(string_listnode));
    new->value = my_strdup(str);
    new->next = NULL;
    return new;
}


string_listnode* string_listnode_push_element(string_listnode* head, char* str)
{
    string_listnode* h = head;
    if(h == NULL)
    {
        h = string_listnode_create_element(str);
        return h;
    }
    else
    {
        while(h->next != NULL)
            h = h->next;

        h->next = string_listnode_create_element(str);
    }
    return head;
}


void switch_string_listnodes(string_listnode* sln1, string_listnode* sln2)
{
    char* temp = sln1->value;
    sln1->value = sln2->value;
    sln2->value = temp;
}


void string_listnode_print(string_listnode* head, char* sep, unsigned int print_at_end)
{
    while(head != NULL)
    {
        printf("%s", head->value);
        
        if(print_at_end == 1 || head->next != NULL)
            printf("%s", sep);

        head = head->next;
    }
}


void string_listnode_free(string_listnode* head)
{
    string_listnode* tmp = head;
    while(head != NULL)
    {
        tmp = head;
        head = head->next;

        free(tmp->value);
        free(tmp);       
    }
}