/*******************************************************************************
 * Name        : sort.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : 6/5/21
 * Description : sort implementation.
 * Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "quicksort.h"

#define ROW_SIZE 1024
#define COL_SIZE 64

void printUsage() {
    printf(
        "Usage: ./sort [-i|-d] filename\n"
        "   -i: Specifies the file contains ints\n"
        "   -d: Specifies the file contains doubles\n"
        "   filename: The file to sort.\n"
        "   No flags defaults to sorting strings.\n");
}

int main(int argc, char **argv) {
    bool i_flag = false, d_flag = false;
    int c, num_files = 0;

    while ((c = getopt(argc, argv, "id")) != -1) {
        switch (c) {
            case 'i':
                i_flag = true;
                break;
            case 'd':
                d_flag = true;
                break;
            case '?':  // Case 2: Invalid Flag | Case 7: Multiple Flags with Invalid Flag
                if (isprint(optopt))
                    fprintf(stderr, "Error: Unknown option `-%c' received.\n", optopt), printUsage();
                else
                    printUsage();

                return EXIT_FAILURE;
        }
    }

    for (int i = optind; i < argc; i++) num_files++;

    if (!i_flag && !d_flag && num_files == 0) {  // Case 1: No input arguments
        printUsage();
        return EXIT_FAILURE;
    }

    if (i_flag && d_flag) {  // Case 6: Multiple Valid Flags
        printf("Error: Too many flags specified.\n");
        return EXIT_FAILURE;
    }

    if (num_files > 1) {  // Case 5: Multiple filenames
        printf("Error: Too many files specified.\n");
        return EXIT_FAILURE;
    }

    if ((i_flag || d_flag) && num_files == 0) {  // Case 4: No filename
        printf("Error: No input file specified.\n");
        return EXIT_FAILURE;
    }

    // pass all input validation, now read file
    FILE *fp;
    if (i_flag || d_flag)  // read integer + double files
        fp = fopen(argv[2], "r");
    else  // read string files
        fp = fopen(argv[1], "r");

    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    // Read in the data.
    size_t length = 0;
    char str[COL_SIZE + 2], *eoln, **array = (char **)calloc(ROW_SIZE, sizeof(char *));

    while (fgets(str, COL_SIZE + 2, fp) != NULL) {
        eoln = strchr(str, '\n');

        if (eoln == NULL)
            str[COL_SIZE] = '\0';
        else
            *eoln = '\0';

        if (strlen(str) != 0)
            array[length] = (char *)malloc((COL_SIZE + 1) * sizeof(char)), strcpy(array[length++], str);
    }

    fclose(fp);  //close file

    int int_elem_sz = sizeof(int),
        dbl_elem_sz = sizeof(double),
        str_elem_sz = sizeof(char *);

    if (i_flag) {  //sort integers
        int int_array[ROW_SIZE];
        for (int i = 0; i < length; i++) int_array[i] = atoi(array[i]);

        quicksort(int_array, length, int_elem_sz, int_cmp);
        for (int i = 0; i < length; i++) printf("%d\n", int_array[i]);
    } else if (d_flag) {  //sort doubles
        double dbl_array[ROW_SIZE];
        for (int i = 0; i < length; i++) dbl_array[i] = atof(array[i]);

        quicksort(dbl_array, length, dbl_elem_sz, dbl_cmp);
        for (int i = 0; i < length; i++) printf("%.6f\n", dbl_array[i]);
    } else {  //sort strings
        quicksort(array, length, str_elem_sz, str_cmp);
        for (int i = 0; i < length; i++) printf("%s\n", array[i]);
    }

    // free up memory
    for (int i = 0; i <= length; i++) free(array[i]);
    if (array) free(array);

    return EXIT_SUCCESS;
}