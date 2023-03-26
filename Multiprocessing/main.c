#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <depth>\n", argv[0]);
        return 1;
    }

    int depth = atoi(argv[1]);

    if (depth < 1) {
        printf("Depth must be greater than 0.\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("child pid = %d ppid = %d\n", getpid(), getppid());

        if (depth > 1) {
            char child_depth[10];
            snprintf(child_depth, sizeof(child_depth), "%d", depth - 1);
            execl(argv[0], argv[0], child_depth, NULL);
        }
    } else {
        wait(NULL);
        printf("parent pid = %d\n", getpid());
        // Linux
        // system("pstree -p");
        // MacOS
        system("pstree -w $PPID");

    }

    return 0;
}