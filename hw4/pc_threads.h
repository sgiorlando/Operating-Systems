#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define         NUMFLAVORS       	4
#define         NUMSLOTS        	22000
#define         NUMCONSUMERS     	50
#define		    NUMPRODUCERS		30
#define			NUMDONUTS			250

/**********************************************************************/
/* SIGNAL WAITER, PRODUCER AND CONSUMER THREAD FUNCTIONS              */
/**********************************************************************/

struct  donut_ring {
	int     	flavor [NUMFLAVORS] [NUMSLOTS];
	int     	outptr [NUMFLAVORS];
	int		in_ptr [NUMFLAVORS];
	int		serial [NUMFLAVORS];
	int		spaces [NUMFLAVORS];
	int		donuts [NUMFLAVORS];
};

	void	*sig_waiter ( void *arg );
	void	*producer   ( void *arg );
	void	*consumer ( void *arg) ;
	void    sig_handler ( int );
