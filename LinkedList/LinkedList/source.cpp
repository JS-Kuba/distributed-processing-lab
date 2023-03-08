#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Node {
    void* data;
    struct Node* next;
};

typedef int (*compare_func)(void*, void*);
typedef void (*print_func)(void*);

void insert(struct Node** head_ref, void* new_data, compare_func compare) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = new_data;
    new_node->next = NULL;


    struct Node* current = *head_ref;
    struct Node* prev = NULL;


    while (current != NULL && compare(current->data, new_data) < 0) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
    else {
        prev->next = new_node;
        new_node->next = current;
    }

}

int compare_int(void* a, void* b) {
    int* pa = (int*)a;
    int* pb = (int*)b;
    return (*pa < *pb) ? -1 : (*pa > *pb);
}

int compare_strings(void* a, void* b) {
    char* x = *(char**)a;
    char* y = *(char**)b;
    return strcmp(x, y);
}

void print_int(void* data) {
    int* x = (int*)data;
    printf("%d\n", *x);
}

void print_string(void* data) {
    char* x = *(char**)data;
    printf("%s\n", x);
}


void print_list(struct Node* node, print_func print) {
    while (node != NULL) {
        print(node->data);
        node = node->next;
    }
}


int main() {
    struct Node* head1 = NULL;

    int a1 = 3, b1 = 1, c1 = 2, d1 = 5, e1 = 4;
    insert(&head1, &a1, compare_int);
    insert(&head1, &b1, compare_int);
    insert(&head1, &c1, compare_int);
    insert(&head1, &d1, compare_int);
    insert(&head1, &e1, compare_int);
    print_list(head1, print_int);


    struct Node* head2 = NULL;

    char* a2 = (char*)malloc(strlen("awfhtrs") + 1);
    strcpy_s(a2, strlen("awfhtrs") + 1, "awfhtrs");
    char* b2 = (char*)malloc(strlen("aab") + 1);
    strcpy_s(b2, strlen("aab") + 1, "aab");
    char* c2 = (char*)malloc(strlen("za") + 1);
    strcpy_s(c2, strlen("za") + 1, "za");
    char* d2 = (char*)malloc(strlen("ftrwgegvw4g") + 1);
    strcpy_s(d2, strlen("ktrwgegvw4g") + 1, "ktrwgegvw4g");
    char* e2 = (char*)malloc(strlen("gwhwrggv") + 1);
    strcpy_s(e2, strlen("gwhwrggv") + 1, "gwhwrggv");


    insert(&head2, &a2, compare_strings);
    insert(&head2, &b2, compare_strings);
    insert(&head2, &c2, compare_strings);
    insert(&head2, &d2, compare_strings);
    insert(&head2, &e2, compare_strings);
    print_list(head2, print_string);



    return 0;
}