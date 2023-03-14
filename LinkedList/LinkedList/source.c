#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_STRING_LENGTH = 300;

struct Node {
    void* data;
    struct Node* next;
};

typedef struct CustomStruct {
    int number;
    char letter;
} CustomStruct;

typedef int (*compare_func)(void*, void*);
typedef void (*print_func)(void*);

void insert(struct Node** head_ref, void* new_data, size_t data_size, compare_func compare) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    void* data_copy = malloc(data_size);
    memcpy(data_copy, new_data, data_size);

    new_node->data = data_copy;
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

int list_length(struct Node* head) {
    int length = 0;
    struct Node* current = head;

    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}

int delete_node(struct Node** head_ref, int position) {
    if (*head_ref == NULL || position < 0 || position >= list_length(*head_ref)) {
        return 0;
    }

    struct Node* current = *head_ref;
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
        *head_ref = current->next;
    }
    else {
        prev->next = current->next;
    }

    free(current->data);
    free(current);
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
    printf("Number: %d, Letter: %c\n", s->number, s->letter);
}

void print_list(struct Node* node, print_func print) {
    while (node != NULL) {
        print(node->data);
        node = node->next;
    }
}

void free_list(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        struct Node* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}

int main() {
    int running = 1;
    int command;
    struct Node* head = NULL;
    int data_type = -1;
    size_t data_size = 0;
    compare_func compare = NULL;
    print_func print = NULL;

    while (running) {
        printf("\nEnter command:\n1 - create linked list,\n2 - insert element,\n3 - print list contents,\n4 - delete element,\n5 - exit\n");
        scanf("%d", &command);

        switch (command) {
        case 1: {
            printf("\nSelect data type:\n1 - int,\n2 - string,\n3 - structure with int and char,\n4 - exit\n");
            scanf("%d", &data_type);
            switch (data_type) {
            case 1: {
                data_size = sizeof(int);
                compare = compare_int;
                print = print_int;
                break;
            }
            case 2: {
                data_size = MAX_STRING_LENGTH * sizeof(char);
                compare = compare_strings;
                print = print_string;
                break;
            }
            case 3: {
                data_size = sizeof(CustomStruct);
                compare = compare_structs;
                print = print_struct;
                break;
            }
            default:
                printf("Invalid data type selected\n");
                break;
            }

            if (head != NULL) {
                free_list(head);
            }

            head = NULL;
            printf("Linked list created\n");
            break;
        }
        case 2: {
            if (data_type == -1) {
                printf("No linked list created yet\n");
                break;
            }

            printf("Enter data to insert:\n");

            switch (data_type) {
            case 1: {
                int* data = malloc(data_size);
                scanf("%d", data);
                insert(&head, data, data_size, compare);
                break;
            }

            case 2: {
                char* data = malloc(data_size);
                scanf("%s", data);
                insert(&head, data, data_size, compare);
                break;
            }

            case 3: {
                CustomStruct* data = malloc(data_size);
                printf("Enter number:\n");
                scanf("%d", &data->number);
                printf("Enter letter:\n");
                scanf(" %c", &data->letter);
                insert(&head, data, data_size, compare);
                break;
            }

            default:
                printf("Invalid data type\n");
                break;
            }

            printf("Data inserted\n");
            break;
        }
        case 3: {
            if (data_type == -1) {
                printf("No linked list created yet\n");
                break;
            }

            if (head == NULL) {
                printf("Linked list is empty\n");
                break;
            }

            printf("List contents:\n");
            print_list(head, print);
            break;
        }
        case 4: {
            if (data_type == -1) {
                printf("No linked list created yet\n");
                break;
            }

            int position;
            printf("Enter position to delete:\n");
            scanf("%d", &position);

            if (delete_node(&head, position)) {
                printf("Node at position %d deleted\n", position);
            }

            break;
        }
        case 5: {
            if (head != NULL) {
                free_list(head);
            }

            printf("Exiting program\n");
            running = 0;
            break;
        }
        default:
            printf("Invalid command\n");
            break;
        }
    }

    return 0;
}