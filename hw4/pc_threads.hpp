#ifndef GLOBALS_H
#define GLOBALS_H

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#define NUMFLAVORS 4
#define NUMSLOTS 500
#define NUMCONSUMERS 50
#define NUMPRODUCERS 30


typedef struct {
    int flavor [NUMFLAVORS] [NUMSLOTS];
    int outptr [NUMFLAVORS];
    int in_ptr [NUMFLAVORS];
    int serial [NUMFLAVORS];
    int spaces [NUMFLAVORS];
    int donuts [NUMFLAVORS];
} DONUT_SHOP;

/*****************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS         */
/*****************************************************************/
void *sig_waiter(void *arg);
void *producer(void *arg);
void *consumer(void *arg);
void sig_handler(int);

#endif
