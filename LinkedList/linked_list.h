#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#define MAX_STRING_LENGTH 300
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*compare_func)(void*, void*);
typedef void (*print_func)(void*);

struct Node {
    void* data;
    struct Node* next;
};

typedef struct CustomStruct {
    int number;
    char letter;
} CustomStruct;

typedef struct ListDesc {
    void* head;
    compare_func comparator;
    print_func print;
} ListDesc;




void insert(ListDesc* ld, void* new_data, size_t data_size);

int list_length(ListDesc* ld);

int delete_node(ListDesc* ld, int position);

int compare_int(void* a, void* b);

int compare_strings(void* a, void* b);

int compare_structs(void* a, void* b);

void print_int(void* data);

void print_string(void* data);

void print_struct(void* data);

void print_list(ListDesc* ld);

void free_list(ListDesc* ld);

#endif