#include"linked_list.h"
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4
#define WORKCOUNT 10000

typedef struct {
    char thread_symbol;
    ListDesc* ld;
} ThreadArgs;

void* thread_insert(void* arg) {
    ThreadArgs* thread_args = (ThreadArgs*)arg;
    ListDesc* ld = thread_args->ld;
    char thread_symbol = thread_args->thread_symbol;

    int i = WORKCOUNT;
    while(i--) {
        CustomStruct* data = malloc(sizeof(CustomStruct));
        data->letter = thread_symbol;
        data->number = i;
        insert(ld, data, sizeof(CustomStruct));
    }
    return NULL;
}

void* thread_delete(void* arg) {
    ThreadArgs* thread_args = (ThreadArgs*)arg;
    ListDesc* ld = thread_args->ld;

    int i = WORKCOUNT;
    while(i--) {
        delete_node(ld, 0);
    }
    return NULL;
}

int main() {
    ListDesc* ld = malloc(sizeof(ListDesc));
    create_list(ld, compare_structs, print_struct);

    // insert element to test if threads delete correct number of list elements
    CustomStruct cs;
    cs.number = WORKCOUNT + 1;
    cs.letter = 'X';
    insert(ld, &cs, sizeof(cs));

    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    // assign thread symbols and list descriptor
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].thread_symbol = '0' + i + 17;
        thread_args[i].ld = ld;
    }

    // insert list elements
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_insert, &thread_args[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // save results to the file
    FILE* fp = freopen("threads_results.txt", "w", stdout);
    print_list(ld);
    fclose(fp);    

    // delete list elements
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_delete, &thread_args[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // print remaining contents of the list
    freopen("/dev/tty", "w", stdout);
    print_list(ld);
    fclose(fp);

    // free the memory
    free_list(ld);

    return 0;
}