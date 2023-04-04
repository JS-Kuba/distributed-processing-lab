#include"linked_list.h"

int create_list(ListDesc* ld, compare_func comparator, print_func printer) {
    ld->head = NULL;
    ld->comparator = comparator;
    ld->print = printer;

    if (sem_init(&ld->semaphore, 0, 1) != 0) {
        perror("semaphore init failed!");
        return 1;
    }
    return 0;
}

void insert(ListDesc* ld, void* new_data, size_t data_size) {
    sem_wait(&ld->semaphore);
    
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    void* data_copy = malloc(data_size);
    memcpy(data_copy, new_data, data_size);

    new_node->data = data_copy;
    new_node->next = NULL;

    struct Node* current = ld->head;
    struct Node* prev = NULL;

    while (current != NULL && ld->comparator(current->data, new_data) < 0) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        new_node->next = ld->head;
        ld->head = new_node;
    }
    else {
        prev->next = new_node;
        new_node->next = current;
    }
    sem_post(&ld->semaphore);
}

int list_length(ListDesc* ld) {

    int length = 0;
    struct Node* current = ld->head;

    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}

int delete_node(ListDesc* ld, int position) {

    sem_wait(&ld->semaphore);

    if (ld->head == NULL || position < 0 || position >= list_length(ld)) {
        return 0;
    }

    struct Node* current = ld->head;
    struct Node* prev = NULL;
    int i = 0;

    while (current != NULL && i < position) {
        prev = current;
        current = current->next;
        i++;
    }

    if (current == NULL) {
        return 0;
    }

    if (prev == NULL) {
        ld->head = current->next;
    }
    else {
        prev->next = current->next;
    }

    free(current->data);
    free(current);
    
    sem_post(&ld->semaphore);

    return 1;
}


int compare_int(void* a, void* b) {
    int* pa = (int*)a;
    int* pb = (int*)b;
    return (*pa < *pb) ? -1 : (*pa > *pb);
}

int compare_strings(void* a, void* b) {
    char* str1 = (char*)a;
    char* str2 = (char*)b;
    return strcmp(str1, str2);
}

int compare_structs(void* a, void* b) {
    struct CustomStruct* s1 = (struct CustomStruct*)a;
    struct CustomStruct* s2 = (struct CustomStruct*)b;
    return compare_int(&s1->number, &s2->number);
}

void print_int(void* data) {
    int* x = (int*)data;
    printf("%d\n", *x);
}


void print_string(void* data) {
    char* str = (char*)data;
    printf("%s\n", str);
}

void print_struct(void* data) {
    struct CustomStruct* s = (struct CustomStruct*)data;
    printf("Number: %d, Thread: %c\n", s->number, s->letter);
}

void print_list(ListDesc* ld) {
    struct Node* node = ld->head;
    print_func print = ld->print;
    while (node != NULL) {
        print(node->data);
        node = node->next;
    }
}

void free_list(ListDesc* ld) {
    struct Node* current = ld->head;
    while (current != NULL) {
        struct Node* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}