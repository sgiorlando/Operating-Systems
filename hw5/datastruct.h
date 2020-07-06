#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {FALSE, TRUE} bool;
typedef struct node node, *list;

struct node {
    int blockSize;
    bool memory_left;
    int serialNum;
    list next;
    list prev;
};

int allocateNode(list* list_pointer, int blockSize, bool memory_left, int serial);
int deleteNode(list* list_pointer, list node);
void destroy(list* list_pointer);
bool emptyList(list new_list);
void freeNode(list* list_pointer);
int initList(list* list_pointer);
int insertAfter(list* list_pointer, list node, int blockSize, bool memory_left, int serial);
list listIterator(list new_list, list lastNode);

#endif
