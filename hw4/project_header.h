//Scott Giorlando
//Assignment 4

#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#define         NUMFLAVORS       	4
#define         NUMSLOTS        	30000
#define         NUMCONSUMERS     	50
#define		NUMPRODUCERS		30
#define		NUM_DONUTS		50


struct  donut_ring {
	int     	flavor [NUMFLAVORS] [NUMSLOTS];
	int     	outptr [NUMFLAVORS];
	int		in_ptr [NUMFLAVORS];
	int		serial [NUMFLAVORS];
	int		spaces [NUMFLAVORS];
	int		donuts [NUMFLAVORS];
};

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

	void	*sig_waiter ( void *arg );
	void	*producer   ( void *arg );
	void	*consumer ( void *arg) ;
	void    sig_handler ( int );
