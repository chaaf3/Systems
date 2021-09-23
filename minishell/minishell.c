/*******************************************************************************
 * Name        : minishell.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : 6/25/21
 * Description : Minishell for linux.
 * Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t answer = false;
void cd(char* path) {
    char* compare = "~";
    if (path == NULL || (strcmp(path, compare) == 0)) {
        struct passwd *pwuid;
        if ((pwuid = getpwuid(getuid())) == NULL) {
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
        }
        chdir(pwuid->pw_dir);
    }
    else if (chdir(path) == -1) {
        fprintf(stderr,"Error: Cannot change directory to '%s'. %s.\n" , path, strerror(errno));
    }
}
void catch_signal(int sig) {
        answer = sig;
}

int main(int argc, char *argv[]) {
    //do signal stuff first
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = catch_signal;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGINT, &act, NULL) < 0) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    while(true) {
        //time to make the minishell text
        char path[PATH_MAX];
        getcwd(path, PATH_MAX);
        printf("[");
        printf("%s%s", BRIGHTBLUE, path);
        printf("%s]$ ", DEFAULT);
        fflush(stdout);
        char buf[PATH_MAX];
        int numRead;
        if ((numRead = read(STDIN_FILENO, buf, PATH_MAX)) < 0) {
            if (errno == EINTR) {
                printf("\n");
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
        buf[numRead - 1] = '\0';
        // now I need to make argv2 and argc2
        int argc2 = 0;
        char *argv2[PATH_MAX];
        char *val = strtok(buf, " ");
        while (val != NULL) {
            argv2[argc2] = val;
            argc2++;
            val = strtok(NULL, " ");
        }
        //deal with exit and cd
        if (strcmp(argv2[0], "cd") == 0) {
            if (argc2 > 2) {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
            }
            else if (argc2 == 1) {
                cd(NULL);
            }
            else {
                cd(argv2[1]);
            }
            continue;
        }

        argv2[argc2] = NULL;

        if (strcmp(argv2[0], "exit") == 0) {
            return EXIT_SUCCESS;
        }
        //now work with forking
        pid_t pid;
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;

        } else if (pid > 0) {
            // We're in the parent.
            // pid is the process id of the child.
            // Wait for the child to complete
            int status;
            pid_t w = waitpid(pid, &status, 0);
            if (w == -1) {
                // wait failed.
                if (errno == EINTR) {
                printf("\n");
                continue;
            }
                fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        else {
            // We're in the child.
            //deal with all other exec's
            if(execvp(argv2[0], argv2) == -1) {
                fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_FAILURE;
}

