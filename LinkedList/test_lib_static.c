#include<stdio.h>
#include"linked_list.h"

int main() {
    size_t data_size = 0;
    compare_func compare = NULL;
    print_func print = NULL;
    ListDesc* ld = malloc(sizeof(ListDesc));

    ld->comparator = compare_int;
    ld->print = print_int;
    data_size = sizeof(int);

    int* data1 = malloc(data_size);
    int* data2 = malloc(data_size);
    
    printf("Enter integer data1:\n");
    scanf("%d", data1);
    printf("Enter integer data2:\n");
    scanf("%d", data2);
    insert(ld, data1, data_size);
    insert(ld, data2, data_size);
    printf("List contents:\n");
    print_list(ld);

    return 0;
}
