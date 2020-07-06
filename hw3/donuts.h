//Scott Giorlando
//Assignment 3

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <unistd.h>

#define SEMKEY (key_t)867532046
#define MEMKEY (key_t)867532046

#define NUMFLAVORS 4
#define NUMSLOTS 50
#define NUMSEMIDS 3
#define PROD 0
#define CONSUMER 1
#define OUTPTR 2

struct donut_ring {
  int flavor [NUMFLAVORS] [NUMSLOTS];
  int outptr [NUMFLAVORS];
};

extern int p(int, int);
extern int v(int, int);
extern int semsetall(int, int, int);
