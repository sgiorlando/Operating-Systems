//Scott Giorlando
//Assignment 2 -- Sort

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  pid_t pid_child;
  int iPipe[2], oPipe[2];
  int count = -1;
  int total = 0;
  int formerArea = 0;
  int currentArea = 1;
  int bytes = 0;
  int sort = 0;
  char str_buf[100], first_name[100], last_name[100];
  FILE *outFile, *dataSort, *tempSort;

  if(argc == 1 || argc > 2) {
    printf("\nEnter the command ./assign2_sort then the file you would like to sort.\n");
    exit(1);
  }

  if(pipe(oPipe) == -1 || pipe(iPipe) == -1) {
    perror("Error: Failed in Parent pipe creation:");
    exit(7);
  }

  pid_child = fork();

  if(pid_child == -1) {
    perror("Failed in fork call/creation of child: ");
    exit(7);
  }
  else if (pid_child == 0) {
    if(close(0) == -1) {
      perror("Error: Child failure in pipe process");
      exit(7);
    }

    if(dup(oPipe[0]) != 0) {
      perror("Error: Child failure in dup process");
      exit(7);
    }

    if(close(1) == -1) {
      perror("Error: Child failure in close process");
      exit(7);
    }

    if(dup(iPipe[1]) != 1) {
      perror("Error: Child failure in dup process");
      exit(7);
    }

    if(close(oPipe[0]) == -1 || close(oPipe[1]) == -1 || close(iPipe[0]) == -1 || close(iPipe[1]) == -1) {
      perror("Error: Child failure in close process");
      exit(7);
    }

    excelp("sort", "sort", "-k3,3", "-k1,1", NULL);

    perror("Error: Child failure in sort process");
    exit(7);
  }

  outFile = fdopen(oPipe[1], "w");

  if((dataSort = fopen(argv[1], "r")) == NULL) {
    perror("Error: Failure when opening file");
    exit(5);
  }

  while(fgets(str_buf, 100, dataSort) != NULL) {
    fprintf(outFile, "%s", str_buf);
  }

  fflush(outFile);
  fclose(outFile);
  close(oPipe[0]);
  close(oPipe[1]);
  close(iPipe[1]);
  fclose(dataSort);

  tempSort = fopen("sortdata.txt", "w+");
  sort = open("sortdata.txt", O_RDWR);

  while(bytes = read(iPipe[0], str_buf, 100) != 0) {
    if(write(sort, str_buf, bytes) == -1) {
      printf("Error: Cannot write to file.\n");
      exit(5);
    }
  }

  while(fgets(str_buf, 100, tempSort) != NULL) {
    sscanf(str_buf, "%s %s %d \n", last_name, first_name, &currentArea);

    if(formerArea == 0) {
      formerArea = currentArea;
      count++;
    }

    if(formerArea == currentArea) {
      count++;
      total++;
    }
    else {
      printf("Area code %d had %d different names\n", formerArea, count);
      formerArea = currentArea;
      count = 1;
      total++;
    }
  }

printf("Area code %d had %d different names\n", formerArea, count);
printf("\n%d lines were processed in this report.\n", total);

fclose(tempSort);
close(iPipe[0]);

return 0;
}
