#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    int status;
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { 
        int tmp_fd;
        // backup stderr
        if ((tmp_fd = dup(STDERR_FILENO)) == -1) {
            perror("dup");
            exit(EXIT_FAILURE);
        }
        // copy stdout to stderr
        if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        // copy the backup of stderr to stdout
        if (dup2(tmp_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(tmp_fd);
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "Failed to execute program\n");
        exit(EXIT_FAILURE);
    } else {
        wait(&status);
    }
    return 0;
}