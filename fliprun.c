#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <io.h>        // _dup, _dup2
  #include <process.h>  // _spawnvp
  #include <fcntl.h>
#else
  #include <unistd.h>
  #include <sys/wait.h>
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    /* ===== Windows / MinGW ===== */

    int tmp_fd;

    /* backup stderr */
    tmp_fd = _dup(2);
    if (tmp_fd == -1) {
        perror("_dup");
        exit(EXIT_FAILURE);
    }

    /* stdout -> stderr */
    if (_dup2(1, 2) == -1) {
        perror("_dup2");
        exit(EXIT_FAILURE);
    }

    /* backup stderr -> stdout */
    if (_dup2(tmp_fd, 1) == -1) {
        perror("_dup2");
        exit(EXIT_FAILURE);
    }

    _close(tmp_fd);

    /* spawn child process */
    int ret = _spawnvp(_P_WAIT, argv[1], (const char * const *)&argv[1]);
    if (ret == -1) {
        perror("_spawnvp");
        exit(EXIT_FAILURE);
    }

#else
    /* ===== POSIX ===== */

    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        int tmp_fd;

        /* backup stderr */
        tmp_fd = dup(STDERR_FILENO);
        if (tmp_fd == -1) {
            perror("dup");
            exit(EXIT_FAILURE);
        }

        /* stdout -> stderr */
        if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        /* backup stderr -> stdout */
        if (dup2(tmp_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        close(tmp_fd);

        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        wait(&status);
    }
#endif

    return 0;
}
