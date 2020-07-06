//Scott Giorlando
//Assignment 4

#ifndef PROJECT_HEADER_HPP
#define PROJECT_HEADER_HPP

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#define NUMFLAVORS 4
#define NUMSLOTS 500
#define NUMCONSUMERS 50
#define NUMPRODUCERS 30
#define NUMDONUTS 200


typedef struct {
    int flavor [NUMFLAVORS] [NUMSLOTS];
    int outptr [NUMFLAVORS];
    int in_ptr [NUMFLAVORS];
    int serial [NUMFLAVORS];
    int spaces [NUMFLAVORS];
    int donuts [NUMFLAVORS];
} donut_ring;

/*****************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS         */
/*****************************************************************/
void *sig_waiter(void *arg);
void *producer(void *arg);
void *consumer(void *arg);
void sig_handler(int);

#endif
