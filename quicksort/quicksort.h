/*******************************************************************************
 * Name        : quicksort.h
 * Author      : Connor Haaf and Ming Lin
 * Date        : 6/5/21
 * Description : Quicksort header file.
 * Pledge      : I pledge my Honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#ifndef QUICKSORT_H_
#define QUICKSORT_H_
#include <stddef.h>

int int_cmp(const void *a, const void *b);
int dbl_cmp(const void *a, const void *b);
int str_cmp(const void *a, const void *b);
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp)(const void *, const void *));

#endif