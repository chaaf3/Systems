#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


void cd(char* path) {
    char* compare = "~";
    if (sizeof(path) == 0 || (strcmp(path, compare) == 0)) {
        chdir(getpwuid(getuid(void)).pw_dir);
    }
    else chdir(path);
}
void catch_signal(int sig) {
    volatile sig_atomic_t variable interrupted;
    if (sig == SIGINT) {
        //do something
        goto EXIT;
    }
}

int main(int argc, char *argv[]) {
    
    //do signal stuff first
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = catch_signal;
    sigemptyset(&act.sa_mask);

    act.sa_flags = SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        fprintf(stderr, "Error: Could not register signal handler. %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    while(true) {
        //now work with forking
        pid_t pid;
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;

        } else if (pid > 0) {
            // We're in the parent.
            // pid is the process id of the child.
            
            int status;
            // Wait for the child to complete
            pid_t w = wait(NULL);
            if (w == -1) {
                // wait failed.
                perror("wait failed");
                exit(EXIT_FAILURE);
            }
        }
        else {
            // We're in the child.
            //deal with exit and cd
            if (argv[0] == "cd") {
                cd(argv[1]);
            }
            //deal with all other exec's
            else (execv("pfind", argv) == -1) {
                fprintf(stderr, "Error: execv() failed. %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_FAILURE;
}

EXIT:
return EXIT_SUCCESS;

