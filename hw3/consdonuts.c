//Scott Giorlando
//Assignment 3

#include "donuts.h"

int main() {
  int i, j, k, rand_num;
  int donut_type;
  int type1, type2, type3, type4;
  int shmid, semid[3];
  int flavors[NUMFLAVORS][12];
  struct tm *ptm;
  struct donut_ring *shared_ring;
  struct timeval randtime;
  char sTime[40];

  if((shmid = shmget(MEMKEY, sizeof(struct donut_ring), IPC_CREAT | 0600)) == -1) {
    perror("shard get failed: ");
    exit(1);
  }

  if((shared_ring = (struct donut_ring *) shmat(shmid, NULL, 0)) == (void *)-1) {
    perror("shared attach failed: ");
    exit(1);
  }

  for(i = 0; i < NUMSEMIDS; i++) {
    if((semid[i] = semget(SEMKEY +i, NUMFLAVORS, IPC_CREAT | 0600)) == -1) {
      perror("semaphore allocation failed: ");
      exit(1);
    }
  }

  for(i = 0; i < 10; i++) {
    type1 = 0, type2 = 0, type3 = 0, type4 = 0;

    for(k = 0; k < 12; k++) {
      gettimeofday(&randtime, (struct timezone *) 0);

      unsigned short xsub[3];
      xsub[0] = (ushort) randtime.tv_usec;
      xsub[1] = (ushort)(randtime.tv_usec >> 16);
      xsub[2] = (ushort)(getpid());
      rand_num = nrand48(xsub) & 3;

      p(semid[OUTPTR], rand_num);
      p(semid[CONSUMER], rand_num);

      donut_type = shared_ring->flavor[rand_num][shared_ring->outptr[rand_num]];

//       switch(rand_num) {

//        case 0:
//        {
//          flavors[rand_num][type1] = donut_type;
//          type1++;
//          break;
//        }

//        case 1:
//        {
//          flavors[rand_num][type2] = donut_type;
//          type2++;
//          break;
//        }

//        case 2:
//        {
//          flavors[rand_num][type3] = donut_type;
//          type3++;
//          break;
//        }

//        case 3:
//        {
//          flavors[rand_num][type4] = donut_type;
//          type4++;
//          break;
//        }
//      }

      if(rand_num == 0) {
        flavos[rand_num][type1] = donut_type;
        type1++;
      } else if(rand_num == 1) {
        flavors[rand_num][type2] = donut_type;
        type2++;
      } else if(rand_num == 2) {
        flavors[rand_num][type3] = donut_type;
      } else if(rand_num == 3) {
        flavors[rand_num][type4] = donut_type;
        type4++;
      }

      if(shared_ring->outptr[rand_num] >= NUMSLOTS) {
        shared_ring->outptr[rand_num] = 0;
      } else {
        shared_ring->outptr[rand_num] = shared_ring->outptr[rand_num] + 1;
      }

      v(semid[PROD], rand_num);
      v(semid[OUTPTR], rand_num);
    }

    ptm = localtime(&randtime.tv_sec);
    strftime(sTime, sizeof(sTime), "%H:%M:%S", ptm);
    long ms = randtime.tv_usec / 100;

    printf("\nProcess PID: %d", getpid());
    printf("\nTime: %s.%ld", sTime, ms);
    printf("\nDozen #: %d\n", i + 1);
    printf("Plain\tJelly\tCoconut\tHoney-dip\n");

    for(j = 0; j < 12; j++) {
      printf("%d\t%d\t%d\t%d\n", flavors[0][j], flavors[1][j], flavors[2][j], flavors[3][j]);
    }

    for(j = 0; j < NUMFLAVORS; j++) {
      for(k = 0; k < 12; k++) {
        flavors[j][k] = 0;
      }
    }
  }
  //fprintf(stderr, " CONSUMER %s DONE\n", argv[1]);
  return 0;
}
