#ifndef _BUDDYSYS_H
#define _BUDDYSYS_H

#include "datastruct.h"

typedef struct treeNode treeNode, *tree;

struct treeNode {
    int blockSize;
    bool memory_left;
    int serialNum;
    int base_adr;
    tree parent;
    tree left;
    tree right;
};

int allocateTreeNode(tree* tree_pointer, int blockSize, bool memory_left, int serial, int base_adr);
void freeTreeNode(tree* tree_pointer);
int initTree(tree* tree_pointer);
bool emptyTree(tree new_tree);
int makeRoot(tree* tree_pointer, int blockSize, bool memory_left, int serial, int base_adr,
    tree parent, tree left, tree right);
void destroyTree(tree* tree_pointer);

int treeFindSerialNum(tree new_tree, tree* node, int size);
int treeFindLargestChunk(tree new_tree, int* largest);
int treeTraverseBest(tree new_tree, tree* best, int size);

#endif
