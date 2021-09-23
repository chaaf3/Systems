/*******************************************************************************
 * Name        : spfind.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : June 16, 2021
 * Description : a sorted file search by permissions.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void printUsage() {
    fprintf(stderr, "Usage: ./spfind -d <directory> -p <permissions string> [-h]\n");
}

int main(int argc, char *argv[]) {
    int pfindToSort[2], sortToParent[2];
    if (pipe(pfindToSort) < 0) {
        fprintf(stderr, "Error: pfind failed.\n");
        return EXIT_FAILURE;
    }

    if (pipe(sortToParent) < 0) {
        fprintf(stderr, "Error: sort failed\n");
        return EXIT_FAILURE;
    }

    pid_t pid[2];
    if ((pid[0] = fork()) == 0) {
        //in the child
        //this is pfind
        int holder;
        close(pfindToSort[0]);
        dup2(pfindToSort[1], STDOUT_FILENO);
        close(sortToParent[0]);
        close(sortToParent[1]);

        if ((holder = execv("./pfind", argv)) == -1) {
            fprintf(stderr, "Error: pfind failed.\n");
            exit(EXIT_FAILURE);
        }
    }

    if ((pid[1] = fork()) == 0) {
        //in the child
        //this is sort
        close(pfindToSort[1]);
        dup2(pfindToSort[0], STDIN_FILENO);
        close(sortToParent[0]);
        dup2(sortToParent[1], STDOUT_FILENO);
        // what does this sort bianary take in?
        if (execlp("sort", "sort", NULL) == -1) {
            fprintf(stderr, "Error: sort failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    //now in parent
    close(sortToParent[1]);
    dup2(sortToParent[0], STDIN_FILENO);
    close(pfindToSort[0]);
    close(pfindToSort[1]);

    char buf[4096];

    int total = 0;
    while (1) {
        ssize_t count = read(STDIN_FILENO, buf, sizeof(buf));

        if (count == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("read()");
                exit(EXIT_FAILURE);
            }
        } else if (count == 0) {
            break;
        } else {
            if (write(STDOUT_FILENO, buf, count) == -1) {
                fprintf(stderr, "Error: Something wrong with write.\n");
                exit(EXIT_FAILURE);
            };
        }

        for (int i = 0; i < count; i++) {
            if (buf[i] == '\n') {
                total++;
            }
        }
    }

    printf("Total matches: %d\n", total);

    close(sortToParent[0]);
    return EXIT_SUCCESS;
}