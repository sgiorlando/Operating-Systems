#include "buddysys.h"

int allocateTreeNode(tree* tree_pointer, int blockSize, bool memory_left, int serial, int base_adr) {
    tree new_tree = (tree)malloc(sizeof(treeNode));
    if (new_tree == NULL) {
        return -1;
    }

    *tree_pointer = new_tree;
    new_tree->blockSize = blockSize;
    new_tree->memory_left = memory_left;
    new_tree->serialNum = serial;
    new_tree->base_adr = base_adr;
    new_tree->parent = NULL;
    new_tree->left = NULL;
    new_tree->right = NULL;

    return 0;
}

void freeTreeNode(tree* tree_pointer) {
    free(*tree_pointer);
    *tree_pointer = NULL;
}

int initTree(tree* tree_pointer) {
    *tree_pointer = NULL;
    return 0;
}

bool emptyTree(tree new_tree) {
    return(new_tree == NULL) ? TRUE : FALSE;
}

int makeRoot(tree* tree_pointer, int blockSize, bool memory_left, int serial, int base_adr,
            tree parent, tree left, tree right) {
    if (emptyTree(*tree_pointer) == FALSE) {
        return -1;
    }
    if (allocateTreeNode(tree_pointer, blockSize, memory_left, serial, base_adr) == -1) {
        return -1;
    }

    (*tree_pointer)->parent = parent;
    (*tree_pointer)->left = left;
    (*tree_pointer)->right = right;

    return 0;
}

void destroyTree(tree* tree_pointer) {
    if (emptyTree(*tree_pointer) == FALSE) {
        destroyTree(&(*tree_pointer)->left);
        destroyTree(&(*tree_pointer)->right);
        freeTreeNode(tree_pointer);
    }
}

int treeFindSerialNum(tree new_tree, tree* node, int size) {
    if (emptyTree(new_tree) == TRUE) {
        return 0;
    }

    if (new_tree->serialNum == size) {
        *node = new_tree;
    } else {
        treeFindSerialNum(new_tree->left, node, size);
        treeFindSerialNum(new_tree->right, node, size);
    }

    return 0;
}

int treeFindLargestChunk(tree new_tree, int* largest) {
    if (emptyTree(new_tree) == TRUE) {
        return 0;
    }

    treeFindLargestChunk(new_tree->left, largest);
    treeFindLargestChunk(new_tree->right, largest);

    if (new_tree->memory_left) {
        if (new_tree->blockSize > *largest) {
            *largest = new_tree->blockSize;
        }
    }

    return 0;
}

int treeTraverseBest(tree new_tree, tree* best, int size) {
    if (emptyTree(new_tree) == TRUE) {
        return 0;
    }

    treeTraverseBest(new_tree->left, best, size);
    treeTraverseBest(new_tree->right, best, size);

    if (new_tree->memory_left && new_tree->blockSize > size) {
        if (*best != NULL && (*best)->blockSize > new_tree->blockSize) {
            *best = new_tree;
        } else if (*best == NULL) {
            *best = new_tree;
        }
    }

    return 0;
}
