/*******************************************************************************
 * Name        : linked_list.h
 * Author      : Connor Haaf and Ming Lin
 * Date        : June 2, 2021
 * Description : Practice with double linked lists.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <string.h>
#include "node.h"

typedef struct linked_list {
    node *head;
    node *tail;
    size_t num_nodes;
} linked_list;

linked_list* create_linked_list() {
    return (linked_list *)calloc(1, sizeof(linked_list));
}

/**
 * Inserts a non-NULL node into a non-NULL linked list.
 * The node is inserted so that the data in the linked list is in
 * non-decreasing order.
 * A node with a value already in the list is inserted AFTER the node(s)
 * already in the list.
 */
void insert_in_order(linked_list *list, node *n,
                     int (*cmp)(const void*, const void*)) {
                if (list->num_nodes == 0) {
                    list->head = n;
                    list->tail = n;
                    n->next = NULL;
                    n->prev = NULL;
                }
                else if (list->num_nodes == 1) {
                    if (cmp(n->data, list->head->data) >= 0) {
                        list->head->next = n;
                        n->prev = list->head;
                        list->tail = n;
                        n->next = NULL;
                    }
                    else {
                        list->tail->prev = n;
                        n->next = list->tail;
                        list->head = n;
                        n->prev = NULL;
                    }
                }
                else {
                    if (cmp(n->data, list->head->data) < 0) {
                        //if n is less than head (replace head)
                        n->next = list->head;
                        n->prev = NULL;
                        list->head->prev = n;
                        list->head = n;
                    }
                    else if (cmp(n->data, list->tail->data) >= 0) {
                        n->prev = list->tail;
                        list->tail->next = n;
                        n->next = NULL;
                        list->tail = n;
                    }
                    else {
                        node* rightNode = list->head;
                        while (cmp(n->data, rightNode->data) >= 0) {
                            rightNode = rightNode->next;
                        }
                        node* leftNode = rightNode->prev;
                        leftNode->next = n;
                        rightNode->prev = n;
                        n->prev = leftNode;
                        n->next = rightNode;
                    }
                }
                list->num_nodes += (size_t)1;


    // TODO
}

void print_list(linked_list *list, void (*print_function)(void*)) {
    putchar('[');
    node *cur = list->head;
    if (cur != NULL) {
        print_function(cur->data);
        cur = cur->next;
    }
    for ( ; cur != NULL; cur = cur->next) {
        printf(", ");
        print_function(cur->data);
    }
    printf("]\n{length: %lu, head->data: ", list->num_nodes);
    list->head != NULL ? print_function(list->head->data) :
                         (void)printf("NULL");
    printf(", tail->data: ");
    list->tail != NULL ? print_function(list->tail->data) :
                         (void)printf("NULL");
    printf("}\n\n");
}

/**
 * Frees a list starting from the tail.
 * This will check if your previous pointers have been set up correctly.
 */
void free_list(linked_list *list, void (*free_data)(void *)) {
    while (list->tail != NULL) {
        node *prev = list->tail->prev;
        free_node(list->tail, free_data);
        list->tail = prev;
    }
}

#endif
