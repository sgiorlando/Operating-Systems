//Scott Giorlando
//Assignment 5
#include "datastruct.h"
#include "buddysys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void firstFit();
void bestFit();
void buddySystem();

int findList(list new_list);
int findlargest_chunk(list new_list);

void printRequests(char* policy);
void printBuddyRequests();

struct request{
    bool is_alloc;
    int size;
    bool is_done;
    int memory_left;
    int links;
    int largest_chunk;
    int actual_size;
    int base_adr;
} req_array[1001];

int memory_left, total_space, total_allocations = 0;
FILE* FileIn;

int main(int argc, char* argv[]) {
    if (argc != 4) {
    printf("Run the program by doing: ./main [Policy] [Free Space] [Input File]\n");
    printf("-----------------------------------------\n");
    printf("Available Choices:\n");
    printf("- buddy = buddy system\n");
    printf("- first = first fit\n");
    printf("- best = best fit\n");
    exit(1);
    }

    memory_left = total_space = atoi(argv[2]) * 1024;
    if((FileIn = fopen(argv[3], "r")) == NULL) {
        printf("Unable to open or find file\n");
        exit(2);
    }

    if (strcmp(argv[1], "buddy") == 0) buddySystem();
    else if (strcmp(argv[1], "first") == 0) firstFit();
    else if (strcmp(argv[1], "best") == 0) bestFit();

    fclose(FileIn);
    return 0;
}

void buddySystem() {
    tree new_tree, best, node, parent;
    int size, serialNum, size_adjust, largest;
    char request_type[10];

    if (initTree(&new_tree) == -1) {
        printf("Failed to create tree\n");
        exit(1);
    }

    makeRoot(&new_tree, memory_left, TRUE, -1, 0, NULL, NULL, NULL);

    while (fscanf(FileIn, "%d %s %d", &serialNum, request_type, &size) != EOF) {
        if (strcmp(request_type, "alloc") == 0) {
            best = NULL;
            treeTraverseBest(new_tree, &best, size);

            size_adjust = total_space;
            while ((size_adjust / 2) > size) {
                size_adjust /= 2;
            }
            if (best != NULL) {
                best->memory_left = FALSE;

                while((best->blockSize / 2) > 32 && (best->blockSize / 2) > size) {
                    makeRoot(&(best->left), best->blockSize / 2, FALSE, -1, best->base_adr, best, NULL, NULL);
                    makeRoot(&(best->right), best->blockSize / 2, TRUE, -1, best->base_adr + (best->blockSize / 2), best, NULL, NULL);
                    best = best->left;
                }
                best->serialNum = serialNum;

                memory_left -= best->blockSize;
                total_allocations++;

                largest = 0;
                treeFindLargestChunk(new_tree, &largest);

                req_array[serialNum].is_alloc = TRUE;
                req_array[serialNum].size = size;
                req_array[serialNum].actual_size = size_adjust;
                req_array[serialNum].is_done = TRUE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].largest_chunk = largest;
                req_array[serialNum].base_adr = best->base_adr;
            } else {
                largest = 0;
                treeFindLargestChunk(new_tree, &largest);

                req_array[serialNum].is_alloc = TRUE;
                req_array[serialNum].size = size;
                req_array[serialNum].actual_size = size_adjust;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].largest_chunk = 0;
                req_array[serialNum].base_adr = -1;

            }
        } else {
            node = NULL;
            treeFindLargestChunkSerialNum(new_tree, &node, size);
            if (node != NULL) {
                node->memory_left = TRUE;
                memory_left += node->blockSize;

                req_array[serialNum].is_alloc = FALSE;
                req_array[serialNum].size = req_array[size].size;
                req_array[serialNum].actual_size = req_array[size].actual_size;
                req_array[serialNum].is_done = TRUE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].base_adr = -1;

                parent = node->parent;
                while (parent != NULL && parent->left->memory_left && parent->right->memory_left) {
                    freeTreeNode(&(parent->left));
                    freeTreeNode(&(parent->right));
                    parent->memory_left = TRUE;
                    parent = parent->parent;
                }

                largest = 0;
                treeFindLargestChunklargest_chunk(new_tree, &largest);

                req_array[serialNum].largest_chunk = largest;
            } else {
                largest = 0;
                treeFindLargestChunklargest_chunk(new_tree, &largest);

                req_array[serialNum].is_alloc = FALSE;
                req_array[serialNum].size = req_array[size].size;
                req_array[serialNum].actual_size = req_array[size].actual_size;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].largest_chunk = largest;
                req_array[serialNum].base_adr = -1;
            }
        }
    }
    printBuddyRequests();
    destroyTree(&new_tree);
}

void firstFit() {
    list new_list, currentList;
    int serialNum, size;
    char request_type[10];

    if (initList(&new_list) == -1) {
        printf("Unable to initialize list\n");
        exit(3);
    }

    insertAfter(&new_list, NULL, memory_left, TRUE, -1);

    while(fscanf(FileIn, "%d %s %d", &serialNum, request_type, &size) != EOF) {
        if(strcmp(request_type, "alloc") == 0) {
            currentList = NULL;

            while ((currentList = listIterator(new_list, currentList)) != NULL) {

                if (currentList->memory_left == TRUE && currentList->blockSize > size) {
                    insertAfter(&new_list, currentList, currentList->blockSize - size, TRUE, -1);

                    currentList->blockSize = size;
                    currentList->memory_left = FALSE;
                    currentList->serialNum = serialNum;

                    memory_left -= size;
                    total_allocations++;

                    req_array[serialNum].is_alloc = TRUE;
                    req_array[serialNum].size = size;
                    req_array[serialNum].is_done = TRUE;
                    req_array[serialNum].memory_left = memory_left;
                    req_array[serialNum].links = findList(new_list);
                    req_array[serialNum].largest_chunk = findlargest_chunk(new_list);

                    break;
                }
            }

            if (currentList == NULL) {
                req_array[serialNum].is_alloc = TRUE;
                req_array[serialNum].size = size;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].links = findList(new_list);
                req_array[serialNum].largest_chunk = findlargest_chunk(new_list);

            }
        } else {
            currentList = NULL;

            while ((currentList = listIterator(new_list, currentList)) != NULL) {
                if (currentList->serialNum == size) {
                    currentList->memory_left = TRUE;
                    currentList->serialNum = -1;

                    memory_left += currentList->blockSize;

                    req_array[serialNum].is_alloc = FALSE;
                    req_array[serialNum].size = currentList->blockSize;
                    req_array[serialNum].is_done = TRUE;
                    req_array[serialNum].memory_left = memory_left;

                    if (currentList->next != NULL && currentList->next->memory_left == TRUE) {
                        currentList->blockSize += currentList->next->blockSize;
                        deleteNode(&new_list, currentList->next);
                    }

                    if (currentList->prev != NULL && currentList->prev->memory_left == TRUE) {
                        currentList->prev->blockSize += currentList->blockSize;
                        deleteNode(&new_list, currentList);
                    }

                    req_array[serialNum].links = findList(new_list);
                    req_array[serialNum].largest_chunk = findlargest_chunk(new_list);

                    break;
                }
            }
            if (currentList == NULL) {
                req_array[serialNum].is_alloc = FALSE;
                req_array[serialNum].size = 0;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].links = findList(new_list);
                req_array[serialNum].largest_chunk = findlargest_chunk(new_list);
            }
        }
    }

    printRequests("First Fit");
    destroy(&new_list);
}

void bestFit() {
    list new_list, currentList, best;
    int serialNum, size;
    char request_type[10];

    if (initList(&new_list) == -1) {
        printf("unable to initialize list\n");
        exit(3);
    }

    insertAfter(&new_list, NULL, memory_left, TRUE, -1);

    while (fscanf(FileIn, "%d %s %d", &serialNum, request_type, &size) != EOF) {
        if (strcmp(request_type, "alloc") == 0) {
            best = currentList = NULL;

            while ((currentList = listIterator(new_list, currentList)) != NULL) {
                if(currentList->memory_left && currentList->blockSize > size) {
                    if(best == NULL || currentList->blockSize < best->blockSize) {
                        best = currentList;
                    }
                }
            }

            if (best != NULL) {
                insertAfter(&new_list, best, best->blockSize - size, TRUE, -1);

                best->blockSize = size;
                best->memory_left = FALSE;
                best->serialNum = serialNum;

                memory_left -= size;
                total_allocations++;

                req_array[serialNum].is_alloc = TRUE;
                req_array[serialNum].size = size;
                req_array[serialNum].is_done = TRUE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].links = findList(new_list);
                req_array[serialNum].largest_chunk = findlargest_chunk(new_list);
            } else {
                req_array[serialNum].is_alloc = TRUE;
                req_array[serialNum].size = size;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].links = findList(new_list);
                req_array[serialNum].largest_chunk = findlargest_chunk(new_list);

            }
        } else {
            currentList = NULL;

            while ((currentList = listIterator(new_list, currentList)) != NULL) {
                if(currentList->serialNum == size) {
                    currentList->memory_left = TRUE;
                    currentList->serialNum = -1;

                    memory_left += currentList->blockSize;

                    req_array[serialNum].is_alloc = FALSE;
                    req_array[serialNum].size = currentList->blockSize;
                    req_array[serialNum].is_done = TRUE;
                    req_array[serialNum].memory_left = memory_left;

                    if(currentList->next != NULL && currentList->next->memory_left == TRUE) {
                        currentList->blockSize += currentList->next->blockSize;
                        deleteNode(&new_list, currentList->next);
                    }

                    if (currentList->prev != NULL && currentList->prev->memory_left == TRUE) {
                        currentList->prev->blockSize += currentList->blockSize;
                        deleteNode(&new_list, currentList);
                    }

                    req_array[serialNum].links = findList(new_list);
                    req_array[serialNum].largest_chunk = findlargest_chunk(new_list);

                    break;
                }
            }
            if (currentList == NULL) {
                req_array[serialNum].is_alloc = FALSE;
                req_array[serialNum].size = 0;
                req_array[serialNum].is_done = FALSE;
                req_array[serialNum].memory_left = memory_left;
                req_array[serialNum].links = findList(new_list);
                req_array[serialNum].largest_chunk = findlargest_chunk(new_list);
            }
        }
    }

    printRequests("Best Fit");
    destroy(&new_list);
}

int findList(list new_list) {
    list currentList = NULL;
    int count = 0;

    while ((currentList = listIterator(new_list, currentList)) != NULL) {
        if (currentList->memory_left) {
            count++;
        }
    }

    return count;
}

int findlargest_chunk(list new_list) {
    list currentList = NULL;
    int largest = 0;

    while ((currentList = listIterator(new_list, currentList)) != NULL) {
        if (currentList->memory_left && currentList->blockSize > largest) {
            largest = currentList->blockSize;
        }
    }

    return largest;
}

void printRequests(char* policy) {
    int i;

    printf("MANAGEMENT POLICY = %s        POOL SIZE = %d KB\n\n", policy, memory_left / 1024);
    printf("TOTAL ALLOCATIONS: %d\n", total_allocations);
    printf("SERIAL     TYPE      SIZE  DONE   TOT-MEM   LINKS  LARGEST-CHUNK\n");
    printf("----------------------------------------------------------------\n");

    for (i = 1; i < 1001; i++) {
        printf("%4d %10s %9d %5s %9d %6d %10d\n", i,
               req_array[i].is_alloc ? "alloc" : "free",
               req_array[i].size,
               req_array[i].is_done ? "YES" : "NO",
               req_array[i].memory_left,
               req_array[i].links,
               req_array[i].largest_chunk);
    }
}

void printBuddyRequests() {
    int i;

    printf("MANAGEMENT POLICY = Buddy System        POOL SIZE = %d KB\n\n", memory_left / 1024);
    printf("TOTAL ALLOCATIONS: %d\n", total_allocations);
    printf("SERIAL   TYPE     SIZE   ACT-SIZE  DONE  BASE-ADR   TOT-MEM  LARGEST-CHUNK\n");
    printf("--------------------------------------------------------------------------\n");

    for (i = 1; i < 1001; i++) {
        printf("%4d %8s %8d %9d %5s %9d %9d %10d\n", i,
               req_array[i].is_alloc ? "alloc" : "free",
               req_array[i].size,
               req_array[i].actual_size,
               req_array[i].is_done ? "YES" : "NO",
               req_array[i].base_adr,
               req_array[i].memory_left,
               req_array[i].largest_chunk);
    }
}
