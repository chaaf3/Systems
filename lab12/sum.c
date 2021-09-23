
/*******************************************************************************
 * Name        : main.c
 * Author      : Connor Haaf & Ming Lin
 * Date        : June 24, 2021
 * Description : 
 * Pledge : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/#include "sum.h"

/**
 * TODO:
 * Takes in an array of integers and its length.
 * Returns the sum of integers in the array.
 */
int sum_array(int *array, const int length) {
    int answer = 0;
    for (int i = 0; i < length; i++) {
        answer += array[i];
    }
    return answer;
}
