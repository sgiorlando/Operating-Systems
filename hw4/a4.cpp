//Scott Giorlando
//Assignment 4

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "project_header.hpp"

using namespace std;

donut_ring shared_ring;
pthread_mutex_t prod [NUMFLAVORS];
pthread_mutex_t cons [NUMFLAVORS];
pthread_cond_t prod_cond [NUMFLAVORS];
pthread_cond_t cons_cond [NUMFLAVORS];
pthread_t thread_id [NUMCONSUMERS + NUMPRODUCERS], sig_wait_id;
int thread_count;

int main(int argc, char* argv[]) {
    int i, j, k, nsigs;
    struct timeval randtime, first_time, last_time;
    struct sigaction new_act;
    int arg_array[NUMCONSUMERS];
    sigset_t all_signals;
    int sigs[] = {SIGBUS, SIGSEGV, SIGFPE};

    pthread_attr_t thread_attr;
    struct sched_param sched_struct;

    /**********************************************************************/
    /* INITIAL TIMESTAMP VALUE FOR PERFORMANCE MEASURE                    */
    /**********************************************************************/

    gettimeofday(&first_time, (struct timezone *) 0);
    for (int i = 0; i < NUMCONSUMERS; i++) {
        arg_array [i] = i + 1; /** SET ARRAY OF ARGUMENT VALUES **/
    }

    /**********************************************************************/
    /* GENERAL PTHREAD MUTEX AND CONDITION INIT AND GLOBAL INIT           */
    /**********************************************************************/

    for (int i = 0; i < NUMFLAVORS; i++) {
        pthread_mutex_init(&prod [i], NULL);
        pthread_mutex_init(&cons [i], NULL);
        pthread_cond_init(&prod_cond [i], NULL); //initialize before use. ALWAYS
        pthread_cond_init(&cons_cond [i], NULL);
        shared_ring.outptr [i] = 0;
        shared_ring.in_ptr [i] = 0;
        shared_ring.serial [i] = 0;
        shared_ring.spaces [i] = NUMSLOTS;
        shared_ring.donuts [i] = 0;
    }



    /**********************************************************************/
    /* SETUP FOR MANAGING THE SIGTERM SIGNAL, BLOCK ALL SIGNALS           */
    /**********************************************************************/

    sigfillset(&all_signals);
    nsigs = sizeof ( sigs) / sizeof ( int);
    for (int i = 0; i < nsigs; i++)
        sigdelset(&all_signals, sigs [i]);
    sigprocmask(SIG_BLOCK, &all_signals, NULL);

    sigfillset(&all_signals);
    for (int i = 0; i < nsigs; i++) {
        new_act.sa_handler = sig_handler;
        new_act.sa_mask = all_signals;
        new_act.sa_flags = 0;
        if (sigaction(sigs[i], &new_act, NULL) == -1) {
            perror("can't set signals: ");
            exit(1);
        }
    }

    printf("just before threads created\n");


    /*********************************************************************/
    /* CREATE SIGNAL HANDLER THREAD, PRODUCER AND CONSUMERS              */
    /*********************************************************************/

    if (pthread_create(&sig_wait_id, NULL,
            sig_waiter, NULL) != 0) {

        printf("pthread_create failed ");
        exit(3);
    }

    pthread_attr_init(&thread_attr);
    pthread_attr_setinheritsched(&thread_attr,
            PTHREAD_INHERIT_SCHED); //inherit from parent (which is default)

#ifdef  GLOBAL
    sched_struct.sched_priority = sched_get_priority_max(SCHED_OTHER);
    pthread_attr_setinheritsched(&thread_attr,
            PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
    pthread_attr_setschedparam(&thread_attr, &sched_struct);
    pthread_attr_setscope(&thread_attr,
            PTHREAD_SCOPE_SYSTEM); //all available cpu if this code is available
#endif

    for (int i = 0; i < NUMCONSUMERS; i++) { //NULL attr
        if (pthread_create(&thread_id [i], NULL,
                consumer, (void *) &arg_array [i]) != 0) {
            printf("pthread_create failed");
            exit(3);
        }
    }
    for (int i = 50; i < NUMCONSUMERS + NUMPRODUCERS; i++) { //
        if (pthread_create(&thread_id [i], NULL,
                producer, (void *) &arg_array [i]) != 0) {
            printf("pthread_create failed");
            exit(3);
        }
    }

    printf("just after threads created\n");

    /*********************************************************************/
    /* WAIT FOR ALL CONSUMERS TO FINISH, SIGNAL WAITER WILL              */
    /* NOT FINISH UNLESS A SIGTERM ARRIVES AND WILL THEN EXIT            */
    /* THE ENTIRE PROCESS....OTHERWISE MAIN THREAD WILL EXIT             */
    /* THE PROCESS WHEN ALL CONSUMERS ARE FINISHED                       */
    /*********************************************************************/
    for (int i = 0; i < NUMCONSUMERS; i++) {

        pthread_join(thread_id [i], NULL);
    }
    /*****************************************************************/
    /* GET FINAL TIMESTAMP, CALCULATE ELAPSED SEC AND USEC           */
    /*****************************************************************/

    int i;
    gettimeofday(&last_time, (struct timezone *) 0);
    if ((i = last_time.tv_sec - first_time.tv_sec) == 0)
        j = last_time.tv_usec - first_time.tv_usec;
    else {
        if (last_time.tv_usec - first_time.tv_usec < 0) {
            i--;
            j = 1000000 +
                    (last_time.tv_usec - first_time.tv_usec);
        } else {
            j = last_time.tv_usec - first_time.tv_usec;
        }
    }
    printf("Elapsed consumer time is %d sec and %d usec\n", i, j);

    printf("\n\n ALL CONSUMERS FINISHED, KILLING  PROCESS\n\n");

    exit(0);

}



/*********************************************/
/* INITIAL PART OF PRODUCER.....             */

/*********************************************/

void *producer(void *arg) {
    int i, j, k;
    unsigned short xsub [3];
    struct timeval randtime;

    gettimeofday(&randtime, (struct timezone *) 0);
    xsub [0] = (ushort) randtime.tv_usec;
    xsub [1] = (ushort) (randtime.tv_usec >> 16);
    xsub [2] = (long) (pthread_self());

    while (1) {
        j = nrand48(xsub) & 3;

        pthread_mutex_lock(&prod [j]);

        while (shared_ring.spaces [j] == 0) {
            pthread_cond_wait(&prod_cond [j], &prod [j]);
        }

        shared_ring.serial[j] = shared_ring.serial[j] + 1;
        shared_ring.flavor[j][(shared_ring.in_ptr[j] % NUMSLOTS)] = shared_ring.serial[j];
        shared_ring.spaces[j] = shared_ring.spaces[j] - 1;
        shared_ring.in_ptr[j] = shared_ring.in_ptr[j] + 1;
        pthread_mutex_unlock(&prod [j]);

        pthread_mutex_lock(&cons[j]);

        shared_ring.donuts[j]++;

        pthread_mutex_unlock(&cons[j]);
        pthread_cond_signal(&cons_cond[j]);

    }

    return NULL;
}
/*********************************************/
/* ON YOUR OWN FOR THE CONSUMER.........     */

/*********************************************/



void *consumer(void *arg) {

    int i, j, k, m, id; rand;
    int donuts[NUMFLAVORS][12]
    int jelly, plain, coconut, honey_dip, consumer;

    unsigned short xsub [3];
    struct timeval randtime;
    id = *(int *) arg;

    consumer = (id % (NUMCONSUMERS)) + 1;

    /* Get a 9 letter file name */
    char *fName = (char *) malloc(sizeof (char) * 10);
    sprintf(fName, "consumer%d", consumer);
    FILE *output_file = fopen(fName, "w");

    gettimeofday(&randtime, (struct timezone *) 0);
    xsub [0] = (ushort) randtime.tv_usec;
    xsub [1] = (ushort) (randtime.tv_usec >> 16);
    xsub [2] = (long) (pthread_self());

    char buf[256];
    struct tm *loctime;
    time_t rawtime;
    rawtime = time(NULL);
    loctime = localtime(&rawtime);
    strftime(buf, 256, "%I:%M %p.", loctime);


    for (int i = 0; i < NUMFLAVORS; i++) {
        for (int j = 0; j < 12; j++) {
            donuts[i][j] = -1;
        }
    }

    for (int i = 1; i <= NUMDONUTS; i++) {

        jelly = 0;
        plain = 0;
        coconut = 0;
        honey_dip = 0;

        for (int m = 0; m < 12; m++) {

            j = nrand48(xsub) & 3;

            //Consuming
            pthread_mutex_lock(&cons[j]);

            while (shared_ring.donuts [j] == 0) {
                pthread_cond_wait(&cons_cond [j], &cons [j]);
            }

            int flavor = shared_ring.flavor[j][shared_ring.outptr[j] % NUMSLOTS];

            switch (j) {
                case 0: //plain
                    donuts[j][jelly] = flavor; //use buffer
                    jelly++;
                    break;
                case 1://jelly
                    donuts[j][plain] = flavor; //use buffer
                    plain++;
                    break;
                case 2://coconut
                    donuts[j][coconut] = flavor; //use buffer
                    coconut++;
                    break;
                case 3://honey-dip
                    donuts[j][honey_dip] = flavor; //use buffer
                    honey++;
                    break;
                default:

                    break;
            }

            shared_ring.outptr[j]++;

            shared_ring.donuts[j]--;

            pthread_mutex_unlock(&cons[j]); //done

            pthread_mutex_lock(&prod[j]);

            shared_ring.spaces[j]++;

            pthread_mutex_unlock(&prod[j]);

            pthread_cond_signal(&prod_cond[j]); //unblock one prod thread waiting

        } //End of 12 dozen loop

        fprintf(output_file, "\nConsumer Number: %d", consumer);
        fprintf(output_file, " Time: %s", buf);
        fprintf(output_file, " Dozen Number: %d\n", dozen_num);

        fprintf(output_file, "Plain\tJelly\tHoney-dip Coconut\n");

        for (int i = 0; i < 12; i++) {
            ostringstream convert1, convert2, convert3, convert4;

            std::string str1(" ");
            std::string str2(" ");
            std::string str3(" ");
            std::string str4("  ");

            convert1 << inner_rep[0][y] <<"\t";
            convert2 << inner_rep[1][y] <<"\t";
            convert3 << inner_rep[2][y] <<"\t";
            convert4 << inner_rep[3][y] <<"\t";

            str1 += convert1.str();
            str2 += convert2.str();
            str3 += convert3.str();
            str4 += convert4.str();

            fprintf(output_file, "%s%s%s%s\n",
                    (inner_rep[0][y] > -1) ? str1.c_str() : "\t",
                    (inner_rep[1][y] > -1) ? str2.c_str() : "\t",
                    (inner_rep[2][y] > -1) ? str3.c_str() : "\t",
                    (inner_rep[3][y] > -1) ? str4.c_str() : "\t");
        }


        for (int i = 0; i < NUMFLAVORS; i++) {
            for (int j = 0; j < 12; j++) {
                donuts[i][j] = -1;
            }
        }//set inner_rep to 0

        /*****************************************************************/
        /* USING A MICRO-SLEEP AFTER EACH DOZEN WILL ALLOW ANOTHER       */
        /* CONSUMER TO START RUNNING, PROVIDING DESIRED RANDOM BEHVIOR   */
        /*****************************************************************/

        usleep(100);
    }//end of 200 dozens

    fclose(output_file);

    return NULL;
}

/***********************************************************/
/* PTHREAD ASYNCH SIGNAL HANDLER ROUTINE...                */

/***********************************************************/

void *sig_waiter(void *arg) {
    sigset_t sigterm_signal;
    int signo;

    sigemptyset(&sigterm_signal);
    sigaddset(&sigterm_signal, SIGTERM);
    sigaddset(&sigterm_signal, SIGINT);

    if (sigwait(&sigterm_signal, & signo) != 0) {
        printf("\n  sigwait ( ) failed, exiting \n");
        exit(2);
    }
    printf("process going down on SIGNAL (number %d)\n\n", signo);
    exit(1);
    return NULL;
}


/**********************************************************/
/* PTHREAD SYNCH SIGNAL HANDLER ROUTINE...                */

/**********************************************************/

void sig_handler(int sig) {
    pthread_t signaled_thread_id;
    int thread_index;

    signaled_thread_id = pthread_self();
    for (int i = 0; i < (NUMCONSUMERS + 1); i++) {
        if (signaled_thread_id == thread_id [i]) {
            thread_index = i;
            break;
        }
    }
    printf("\nThread %d took signal # %d, PROCESS HALT\n",
            thread_index, sig);
    exit(1);
}
