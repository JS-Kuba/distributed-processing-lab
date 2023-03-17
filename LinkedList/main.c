#include"linked_list.h"


int main() {
    int running = 1;
    int command;
    int data_type = -1;
    size_t data_size = 0;
    compare_func compare = NULL;
    print_func print = NULL;
    ListDesc* ld = malloc(sizeof(ListDesc));

    while(running) {
        printf("\nEnter command:\n1 - create linked list,\n2 - insert element,\n3 - print list contents,\n4 - delete element,\n5 - exit\n");
        scanf("%d", &command);

        switch (command) {
            case 1: {
                printf("\nSelect data type:\n1 - int,\n2 - string,\n3 - structure with int and char,\n4 - exit\n");
                scanf("%d", &data_type);
                switch (data_type) {
                    case 1: {
                        ld->comparator = compare_int;
                        ld->print = print_int;
                        data_size = sizeof(int);
                        break;
                    }
                    case 2: {
                        ld->comparator = compare_strings;
                        ld->print = print_string;
                        data_size = MAX_STRING_LENGTH * sizeof(char);
                        break;
                    }
                    case 3: {
                        ld->comparator = compare_structs;
                        ld->print = print_struct;
                        data_size = sizeof(CustomStruct);
                        break;
                    }
                    default:
                        printf("Invalid data type selected\n");
                        break;
                }

                if (ld->head != NULL) {
                    free_list(ld->head);
                }

                ld->head = NULL;
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
                        insert(ld, data, data_size);
                        break;
                    }

                    case 2: {
                        char* data = malloc(data_size);
                        scanf("%s", data);
                        insert(ld, data, data_size);
                        break;
                    }

                    case 3: {
                        CustomStruct* data = malloc(data_size);
                        printf("Enter number:\n");
                        scanf("%d", &data->number);
                        printf("Enter letter:\n");
                        scanf(" %c", &data->letter);
                        insert(ld, data, data_size);
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

                if (ld->head == NULL) {
                    printf("Linked list is empty\n");
                    break;
                }

                printf("List contents:\n");
                print_list(ld);
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

                if(delete_node(ld, position)) {
                    printf("Node at position %d deleted\n", position);
                }
                else {
                    printf("Failed to delete node at position %d", position);
                }

                break;
            }
            case 5: {
                if (ld->head != NULL) {
                    free_list(ld);
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