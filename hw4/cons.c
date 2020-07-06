void *consumer(void *arg) {

    /***** main variables *****/

    /* holds id# of current thread */
    int id;

    /* variables used in random number generation */
    struct timeval randtime;
    unsigned short xsub [3];

    /* dozen[][]: stores a dozen donuts
     * dozen[0] is the donut type
     * dozen[1] is the donut id # */
    int dozen[2][12];

    /* variables for file manipulation */
    FILE *outfp;
    char fname[15];

    /* counter and temp variables */
    int i, j, k, m;

    /* display consumer startup information */
    id = *(int *) arg;
    /* get file ready for output */
       sprintf(fname, "consumer%d.txt", id);
       if ((outfp = fopen(fname, "w")) == NULL) {
           fprintf(stderr, "Can't open file to write consumer%d data!\n", id);
       }

       /* generate seed to be used in random number generator.
          use microsecond component for uniqueness */
       gettimeofday(&randtime, (struct timezone *) 0);
       xsub [0] = (ushort) randtime.tv_usec;
       xsub [1] = (ushort) (randtime.tv_usec >> 16);
       xsub [2] = (ushort) (pthread_self());

       for (i = 0; i < DOZENS; i++) {
   	/* pick 12 donuts */
           for (m = 0; m < 12; m++) {

               /* j = random donut type */
               j = nrand48(xsub) & 3;

               /* grab consumer mutex, sleep if no donuts */
               pthread_mutex_lock(&cons[j]);
               while (shared_ring.donuts[j] == 0) {
                   pthread_cond_wait(&cons_cond[j], &cons[j]);
               }

               /* take a donut from the box, adjust out_ptr, and donuts */
               dozen[0][m] = j;
               dozen[1][m] = shared_ring.flavor[j][shared_ring.out_ptr[j]];
               shared_ring.out_ptr[j] = ( shared_ring.out_ptr[j] + 1 ) % NUMSLOTS;
               shared_ring.donuts[j]--;
               pthread_mutex_unlock(&cons [j]);

               /* get producer mutex, increment space[j], signal waiting producers */
               pthread_mutex_lock(&prod[j]);
               shared_ring.spaces[j]++;
               pthread_mutex_unlock(&prod[j]);
               pthread_cond_signal(&prod_cond[j]);
           }

           usleep(100000); /* sleep 1 ms */
               }

               /* close the output file */
               if ( outfp != NULL )
                   close(outfp);

               return NULL ;
           } /* end consumer thread function */
