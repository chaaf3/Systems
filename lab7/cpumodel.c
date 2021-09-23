/*******************************************************************************
 * Name        : cpumodel.c
 * Author      : Ming Lin & Connor Haaf
 * Date        : June 15, 2021
 * Description : program that finds the cpu in a computer.
 * Pledge : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
    if (strlen(str) < strlen(prefix)) {
        return false;
    }
    for (int i = 0; i < strlen(prefix); i++) {
      if (*(str + i) != *(prefix + i)) {
        return false;
      }
    }
    return true;
}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading, redirecting errors to /dev/null.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
      FILE *fp = popen("cat /proc/cpuinfo", "r");
      if(fp == NULL) {
        printf("ERROR: popen() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
      }

    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
    char buf[256];
    while (fgets(buf, 256, fp) != NULL) {
      char* pref= "model name";

      if (starts_with((char *)(buf), pref)) {
          pref = strchr(buf, ':');
          pref += 2;
        printf("%s", pref);
        break;
      }
    }



    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
    int status = pclose(fp);
    if (status == -1) {
      fprintf(stderr, "Error: pclose() failed. %s. \n", strerror(errno));
      exit(EXIT_FAILURE);
    }


    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
