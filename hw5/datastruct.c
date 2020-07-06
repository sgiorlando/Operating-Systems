#include "datastruct.h"
#include <stdlib.h>
#include <stdio.h>

int allocateNode(list* list_pointer, int blockSize, bool memory_left, int serial){
    list new_list = (list)malloc(sizeof(node));

    if (new_list == NULL) {
        return -1;
    }

    *list_pointer = new_list;
    new_list->blockSize = blockSize;
    new_list->memory_left = memory_left;
    new_list->serialNum = serial;

    return 0;
}

int deleteNode(list* list_pointer, list node) {
    if (emptyList(*list_pointer)) {
        return -1;
    }

    list prev = node->prev;
    list next = node->next;

    if (*list_pointer == node) {
        *list_pointer = (*list_pointer)->next;
        (*list_pointer)->prev = NULL;
    } else {
        if (prev == NULL) {
            return -1;
        } else {
            prev->next = next;
            if (next != NULL) {
                next->prev = prev;
            }
        }
    }
    freeNode(&node);
    return 0;
}

void destroy(list* list_pointer) {
    if (!emptyList(*list_pointer)) {
        destroy(&(*list_pointer)->next);
        freeNode(list_pointer);
    }
}

bool emptyList(list new_list) {
    return (new_list == NULL) ? TRUE : FALSE;
}

void freeNode(list* list_pointer) {
    free(*list_pointer);
    *list_pointer = NULL;
}

int initList(list* list_pointer) {
    *list_pointer = NULL;
    return 0;
}

int insertAfter(list* list_pointer, list node, int blockSize, bool memory_left, int serial) {
    list new_list, next;

    if (allocateNode(&new_list, blockSize, memory_left, serial) == -1) {
        return -1;
    }

    if (emptyList(*list_pointer)) {
        new_list->next = new_list->prev = NULL;
        *list_pointer = new_list;
    } else {
        next = node->next;
        node->next = new_list;
        new_list->prev = node;
        new_list->next = next;
        if (next != NULL) {
            next->prev = new_list;
        }
    }
    return 0;
}

list listIterator(list new_list, list lastNode) {
    return (lastNode == NULL) ? new_list : lastNode->next;
}
