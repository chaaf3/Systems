/*******************************************************************************
 * Name        : snooze.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : June 16, 2021
 * Description : 
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* TODO: Implement signal handler */
bool answer = true;
void catch_signal(int sig) {
    if (sig == SIGINT) {
        answer = false;
    }
}

/**
 * Description:
 * The 'snooze' program takes in a single parameter, which is the number
 * of seconds (no less than 1) the program should sleep.
 *
 * It catches the SIGINT signal. When doing so, it should stop sleeping and
 * print how long it actually slept.
 *
 * For example, if the user runs "./snooze 5" and then types CTRL+C after 3
 * seconds, the program should output:
 * Slept for 3 of the 5 seconds allowed.
 *
 * If the user runs "./snooze 5" and never types CTRL+C, the program should
 * output:
 * Slept for 5 of the 5 seconds allowed.
 */
int main(int argc, char *argv[]) {
    // TODO: Print the usage message "Usage: %s <seconds>\n" and return in
    // failure if the argument <seconds> is not present.

    if (argc != 2) {
        printf("Usage: %%s <seconds>\n");
        return EXIT_FAILURE;
    }

    int holder = atoi(argv[1]);

    if (holder <= 0) {
        printf("Error: Invalid number of seconds '%s' for max snooze time.\n", argv[1]);
        return EXIT_FAILURE;
    }

    // TODO: Parse the argument, and accept only a positive int. If the argument
    // is invalid, error out with the message:
    // "Error: Invalid number of seconds '%s' for max snooze time.\n",
    // where %s is whatever argument the user supplied.

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = catch_signal;
    sigemptyset(&act.sa_mask);
    /* If a blocking system call was interrupted (think "read()"), the system
* call is restarted. If it fails to restart, then the system call fails
* and errno is set to EINTR. */
    act.sa_flags = SIGINT;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        fprintf(stderr, "Error: Could not register signal handler. %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    int i = 0;
    while (i < holder && answer) {
        i++;
        sleep(1);
    }

    // TODO: Create a sigaction to handle SIGINT.

    // TODO: Loop and sleep for 1 second at a time, being able to stop looping
    // when the signal is processed.

    printf("Slept for %d of the %d seconds allowed.\n", i, holder);
    return EXIT_SUCCESS;
}