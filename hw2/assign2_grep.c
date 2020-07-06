//Scott Giorlando
//Assignment 2 -- grep

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  pid_t pid_child;
  int iPipe[2], oPipe[2];
  int count = 0;
  int bytes = 0;
  int grep = 0;
  char str_buf[100];
  FILE* outFile, dataGrep, tempGrep;

  if(argc == 1 || argc > 2) {
    printf("\nEnter the command ./assign2_grep then the file you would like to grep.\n");
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
      perror("Error: Child failure in pipe process");
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

    excelp("grep", "grep", "123", NULL);

    perror("Error: Child failure in grep process");
    exit(7);
  }

  outFile = fdopen(oPipe[1], "w");

  if(dataGrep = fopen(argv[1], "r") == NULL) {
    perror("Error: Failure when opening file");
    exit(5);
  }

  while(fgets(str_buf, 100, dataGrep) != NULL) {
    fprintf(outFile, "%s", str_buf);
  }

  fflush(outFile);
  fclose(outFile);
  close(oPipe[0]);
  close(iPipe[1]);
  fclose(dataGrep);

  tempGrep = fopen("grepdata.txt", "w+");
  grep = open("grepdata.txt", O_RDWR);

  while(bytes = read(iPipe[0], str_buf, 100) != 0) {
    if(write(grep, str_buf, bytes) == -1) {
      printf("Error: Cannot write to file.\n");
      exit(5);
    }
  }

  while(fgets(str_buf, 100, tempGrep) != NULL) {
    count++;
  }

printf("\n%d lines were matched with grep.\n", count);

fclose(tempGrep);
close(iPipe[0]);

return 0;
}
