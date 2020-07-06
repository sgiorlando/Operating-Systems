// Scott Giorlando
// Assignment 1

// need these include files

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>

typedef union {
	int exit_status;
	struct {
		unsigned sig_num: 7;
		unsigned core_dmp: 1;
		unsigned exit_num: 8;
	} parts;
} LE_Wait_Status;

int counter = 0, counter_2G = 0;

void sig_handler(int signum) {
	printf("\nCHILD PROCESS: Awake in handler.\n");

	if (execl("A1", "", (char*)NULL) == -1) {
		perror("\nError: Execl() failure!\n\n");
		exit(7);
	}
}

int main(void)
{

// some local variables

	LE_Wait_Status exit_union;
  pid_t pid, ppid, pid_child;
  int   ruid, rgid, euid, egid;
  int   priority;
  char  msg_buf[100];
  int   msg_pipe[2];
	char	str_buf[] = "Test String\n";

// use the pipe() system call to create the pipe

  if(pipe(msg_pipe) == -1) {
      perror("Error: Failed in Parent pipe creation:");
      exit(7);
  }

// use various system calls to collect and print process details

  printf("\nThis is the Parent process report:\n");
  pid  = getpid();
  ppid = getppid();
  ruid = getuid();
  euid = geteuid();
  rgid = getgid();
  egid = getegid();
  priority = getpriority(PRIO_PROCESS, 0);

	//Prints out the parent program ID's
  printf("\nPARENT PROG:  Process ID is:\t\t%d\n", pid);
  printf("PARENT PROC:  Process parent ID is:\t%d\n", ppid);
  printf("PARENT PROC:  Real UID is:\t\t%d\n", ruid);
  printf("PARENT PROC:  Real GID is:\t\t%d\n", rgid);
  printf("PARENT PROC:  Effective UID is:\t\t%d\n", euid);
  printf("PARENT PROC:  Effective GID is:\t\t%d\n", egid);
  printf("PARENT PROC:  Process priority is:\t%d\n", priority);

	printf("\nPARENT PROC: will now create child, write pipe,\n \
        and do a normal termination\n");

  sprintf(msg_buf, "This is the pipe message from the parent with PID %d", pid);

  switch (pid_child = fork()) {
  case -1:
  perror("Failed in fork call/creation of child: ");
  exit(1);
  case 0:
	{
		sigset_t mask, proc_mask;
		struct sigaction new_action;

    sigemptyset(&proc_mask);
	  sigprocmask(SIG_SETMASK, &proc_mask, NULL);

	  sigemptyset(&mask);

	  new_action.sa_mask = mask;
	  new_action.sa_handler = sig_handler;
	  new_action.sa_flags = 0;

	  if(sigaction(SIGTERM, &new_action, NULL) == -1) {
			perror("\nError: Sigaction failure\n");
			exit(7);
	  }

    printf("\nThis is the Child process report:\n");

    pid  = getpid();
    ppid = getppid();
    ruid = getuid();
    euid = geteuid();
    rgid = getgid();
    egid = getegid();
    priority = getpriority(PRIO_PROCESS, 0);

    printf("\nCHILD PROC:  Process ID is:\t\t%d\n", pid);
    printf("CHILD PROC:  Process parent ID is:\t%d\n", ppid);
    printf("CHILD PROC:  Real UID is:\t\t%d\n", ruid);
    printf("CHILD PROC:  Real GID is:\t\t%d\n", rgid);
    printf("CHILD PROC:  Effective UID is:\t\t%d\n", euid);
    printf("CHILD PROC:  Effective GID is:\t\t%d\n", egid);
    printf("CHILD PROC:  Process priority is:\t%d\n", priority);

	  close(msg_pipe[0]);

	  if(write(msg_pipe[1], str_buf, (strlen(str_buf) + 1)) == - 1) {
			perror("\nError: Write failure\n");
			exit(7);
	  }

	  while (counter_2G < 10) {
			counter++;

			if(counter < 0) {
				counter = 0;
				counter_2G++;
			}
	  }

		write(1, "CHILD: Timed out after 20 Billion iterations\n", 51);
	  exit(7);
	}
	default:
 	 close(msg_pipe[1]);

	   if(read(msg_pipe[0], msg_buf, sizeof(msg_buf)) == -1) {
			perror("\nError: Read failure\n");
			exit(7);
	   }

	  if(kill(pid_child, SIGTERM) == -1) {
			perror("\nError: Kill failure\n");
			exit(7);
		}

	 if((pid = wait(&exit_union.exit_status)) == -1) {
		 perror("\nError: Wait failure\n");
		 exit(7);
	 }

	 printf("Child %d terminated with: ", pid);

	 if(exit_union.parts.sig_num == 0) {
		 printf("exit code %d and ", exit_union.parts.exit_num);
	 }
	 else {
		 printf("signal code %d and ", exit_union.parts.sig_num);
	 }

	 if(exit_union.parts.core_dmp == 0) {
		 printf("no core dump generated.\n");
	 }
	 else {
		 printf("generated a core dump.\n");
	 }

  }

 return;
}
