/*******************************************************************************
 * Name        : nforks.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : 6/5/21
 * Description : Creates n randomsleep processes and waits for them all to end.
 * Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Determines whether or not the input string represents a valid integer.
 * A valid integer has an optional minus sign, followed by a series of digits
 * [0-9].
 */
bool is_integer(char *input) {
    int start = 0, len = strlen(input);

    if (len >= 1 && input[0] == '-') {
        if (len < 2) {
            return false;
        }
        start = 1;
    }
    for (int i = start; i < len; i++) {
        if (!isdigit(input[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Takes as input a string and an in-out parameter value.
 * If the string can be parsed, the integer value is assigned to the value
 * parameter and true is returned.
 * Otherwise, false is returned and the best attempt to modify the value
 * parameter is made.
 */
bool get_integer(char *input, int *value) {
    long long long_long_i;
    if (sscanf(input, "%lld", &long_long_i) != 1) {
        return false;
    }
    *value = (int)long_long_i;
    if (long_long_i != (long long)*value) {
        fprintf(stderr, "Warning: Integer overflow with '%s'.\n", input);
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num forks>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int num_forks = 0;
    if (!is_integer(argv[1]) || !get_integer(argv[1], &num_forks) ||
        num_forks <= 0) {
        fprintf(stderr, "Error: Invalid number of forks '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    /*
      TODO:
      Use fork() and execl().
      Create num_forks children processes.
      If any call to fork fails, an error message should be printed and
      the loop should continue.
      Keep track of how many children were actually spawned.
      The error message will be of the form:
      Error: fork[fork_index] failed. %s.
         fork_index will be an integer [1, ..., num_forks]
         %s will be strerror(errno)
      The children should execute randomsleep with no arguments.
      If execl fails, an error message should be printed and
      EXIT_FAILURE should returned.
      The error message will be of the form:
      Error: execl() failed for pid %ld. %s.
         %s will be strerror(errno)
    */

    int i, numStarted;
    numStarted = 0;
    pid_t pid[num_forks];
    for (i = 0; i < num_forks; i++) {
        if ((pid[i] = fork()) < 0) {
            printf("Error: fork[%d] failed. %s.\n", i, strerror(errno));
        } else if (pid[i] == 0) {
            // try different compilable files
            printf("execing random # %d\n", i);
            if (execl("./randomsleep", "./randomsleep", NULL) < 0) {
                printf("Error: execl() failed for pid %d. %s.", pid[i], strerror(errno));
                return EXIT_FAILURE;
            }
        }
        else {
            numStarted++;
        }
    }

    /*
      TODO:
      Use wait().
      We are in the parent here. Wait for all children that were actually
      spawned. As each child terminates, print the message:
      Child with pid <pid> has terminated.
      If an error occurs during waiting, instead print the message:
      Error: wait() failed. %s.
      %s will be strerror(errno)
    */

    for (i = 0; i < numStarted; i++) {
        //look over the different types of wait
        pid_t cpid = wait(NULL);
        // try different exit statuses

        if (cpid < 0) {
            printf("Error: wait() failed. %s.", strerror(errno));
        } else {
            printf("Child with pid %d has terminated.\n", cpid);
        }
    }

    printf("All children are done sleeping.\n");
    return EXIT_SUCCESS;
}