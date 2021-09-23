/*******************************************************************************
 * Name        : mtsieve.c
 * Author      : Connor Haaf and Ming Lin
 * Date        : 7/1/21
 * Description : 
 * Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

int total_count = 0;
pthread_mutex_t lock;

typedef struct arg_struct {
    int start;
    int end;
} thread_args;

int threes(int answer) {
    int threes = 0;
    while (answer != 0) {
        if ((answer % 10) == 3) {
            threes++;
        }
        answer /= 10;
    }
    return threes;
}

void* stdSieve(void* threadVar) {
    thread_args value = *(thread_args*)threadVar;
    bool* lowPrimes = (bool*)malloc((sizeof(bool)) * (sqrt(value.end) + 1));
    lowPrimes[0] = false;
    lowPrimes[1] = false;
    for (int p = 2; p <= sqrt(value.end); p++) {
        lowPrimes[p] = true;
    }
    int j;
    for (int p = 2; p < sqrt(sqrt(value.end)) + 1; p++) {
        if (lowPrimes[p]) {
            j = p * p;
            while (j < sqrt(value.end) + 1) {
                lowPrimes[j] = 0;
                j = j + p;
            }
        }
    }
    /**********************************************************/
    //end of the standard sieve start of segmented sieve

    bool* highPrimes = (bool*)malloc((sizeof(bool)) * (value.end - value.start + 1));
    bool* pointsetter = highPrimes;
    for (int g = 0; g < value.end - value.start + 1; g++) {
        *pointsetter = true;
        pointsetter++;
    }
    int a = value.start;
    for (int p = 2; p < sqrt(value.end); p++) {
        if (lowPrimes[p]) {
            int i = ceil(value.start / p) * p - value.start;
            if (a <= p) {
                i = i + p;
            }
            for (int k = i; k < value.end - value.start; k += p) {
                highPrimes[p] = false;
            }
        }
    }
    int number = 0;
    int howmany = 0;
    for (int i = 0; i < value.end - value.start; i++) {
        if (highPrimes[i]) {
            number = threes(i + value.start);
            if (number >= 2) {
                howmany++;
            }
        }
    }
    pthread_mutex_lock(&lock);
    total_count += howmany;
    pthread_mutex_unlock(&lock);
    free(lowPrimes);
    free(highPrimes);
    pthread_exit(NULL);
    return threadVar;
}

void printUsage() {
    printf("Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
}

int main(int argc, char* argv[]) {
    bool e_flag = false, s_flag = false, t_flag = false;
    int c = 0, low = -1, high = -1, numberProcessors = 0;

    while ((c = getopt(argc, argv, "e:s:t:")) != -1) {
        switch (c) {
            case 'e':
                if ((high = atoi(optarg)) == 0) {
                    printf("Error: Invalid input '%s' received for parameter '-e'.\n", optarg);
                    return EXIT_FAILURE;
                }
                e_flag = true;
                break;

            case 's':
                if ((low = atoi(optarg)) == 0) {
                    printf("Error: Invalid input '%s' received for parameter '-s'.\n", optarg);
                    return EXIT_FAILURE;
                }
                s_flag = true;
                break;

            case 't':
                if ((numberProcessors = atoi(optarg)) == 0) {
                    printf("Error: Invalid input '%s' received for parameter '-t'.\n", optarg);
                    return EXIT_FAILURE;
                }
                t_flag = true;
                break;

            case '?':
                printf("optopt = %c\n", optopt);

                if (optopt == 'e' || optopt == 's' || optopt == 't') {
                    fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
                }

                return EXIT_FAILURE;
        }
    }

    printf("low: %d, high: %d, numberProcessors: %d\n", low, high, numberProcessors);
    printf("e_flag: %d, s_flag: %d, t_flag: %d\n", e_flag, s_flag, t_flag);

    if (!e_flag && !s_flag && !t_flag) {
        printUsage();
        return EXIT_FAILURE;
    }

    if (!e_flag) {
        printf("Error: Required argument <ending value> is missing.\n");
        return EXIT_FAILURE;
    }

    if (!t_flag) {
        printf("Error: Required argument <num value> is missing.\n");
        return EXIT_FAILURE;
    }

    if (numberProcessors > 2 * get_nprocs()) {
        printf("there aren't that many cores\n");
        return EXIT_FAILURE;
    }
    printf("Finding all prime numbers between %d and %d.\n", low, high);
    int range = (high - low) + 1;
    int numThreads = numberProcessors;
    int rangeSize, remainder, currentStartValue;
    currentStartValue = low;
    remainder = range % numThreads;
    rangeSize = range / numThreads;
    thread_args threads[numThreads];
    pthread_t tid[numThreads];
    for (int i = 0; i < numThreads; i++) {
        threads[i].start = currentStartValue;
        threads[i].end = currentStartValue + rangeSize - 1;
        currentStartValue += rangeSize;
        if (remainder != 0) {
            threads[i].end++;
            currentStartValue++;
            remainder--;
        }
        //now we have the correct arguements make the thread
        if (i == 0) {
            printf("%d segments:\n", numThreads);
        }
        printf("[%d, %d]\n", threads[i].start, threads[i].end);
        pthread_create(&tid[i], NULL, stdSieve, &threads[i]);
        // for (int i = 0; i < numThreads; i++) {
        //     pthread_join(tid[i], NULL);
        // }
        //todo printing the ranges and number of 3-primes
    }
    for (int i = 0; i < numThreads; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("Total primes between %d and %d with two or more '3' digits: %d\n", low, high, total_count);
    return EXIT_SUCCESS;
}