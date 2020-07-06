#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <owner.h>
#include <grp.h>
#include <dirent.h>
#include <time.h>

void printDetails(struct stat statBuffer);

int main(int argc, char *argv[]) {

  struct stat statBuffer;
  struct dirent* entry;
  DIR* directory;
  int i, j;

  if (argc > 1) {

    for(i = 1; i < argc; i++){
      if(lstat(argv[i], &statBuffer) == -1){
	printf("Error: Could not access file on line: %s", argv[i]);
	exit(2);
      }

      printf("File: %s\n", argv[i]);
      printDetails(statBuffer);

    }
  } else {
    if((directory = opendir(".")) == NULL) {
      printf("Error: opendir failure.\n");
      exit(3);
    }

    while((entry = readdir(directory)) != NULL) {
      if(lstat(entry -> d_name, &statBuffer) == -1) {
        printf("ErrorL Lstat failure.\n");
        exit(2);
      }

      printf("File: %s\n", entry -> d_name);
      printDetails(statBuffer);
    }
  }
  return 0;
}

void printDetails(struct stat statBuffer) {

  char permissisons[10];
  struct passwd* owner;
  struct group* group;
  int i;

  if(S_ISDIR(statBuffer.st_mode)){
    printf("Type: directory\n");
  }

  if(S_ISCHR(statBuffer.st_mode)){
    printf("Type: character\n");
  }

  if(S_ISBLK(statBuffer.st_mode)){
    printf("Type: block\n");
  }

  if(S_ISREG(statBuffer.st_mode)){
    printf("Type: regular file\n");
  }

  if(S_ISFIFO(statBuffer.st_mode)){
    printf("Type: pipe\n");
  }

  if(S_ISLNK(statBuffer.st_mode)){
    printf("Type: link\n");
  }

  if(S_ISSOCK(statBuffer.st_mode)){
    printf("Type: socket\n");
  }

  permissisons[9] = '\0';

  for(i = 0; i < 9; i++){
    permissisons[i] = '-';
  }

  if(statBuffer.st_mode & S_IRUSR){
    permissisons[0] = 'r';
  }

  if(statBuffer.st_mode & S_IWUSR){
    permissisons[1] = 'w';
  }

  if(statBuffer.st_mode & S_IXUSR){
    permissisons[2] = 'x';
  }

  if(statBuffer.st_mode & S_IRGRP){
    permissisons[3] = 'r';
  }

  if(statBuffer.st_mode & S_IWGRP){
    permissisons[4] = 'w';
  }

  if(statBuffer.st_mode & S_IXGRP){
    permissisons[5] = 'w';
  }

  if(statBuffer.st_mode & S_IWOTH){
    permissisons[6] = 'r';
  }

  if(statBuffer.st_mode & S_IROTH){
    permissisons[7] = 'w';
  }
  if(statBuffer.st_mode & S_IXOTH){
    permissisons[8] = 'x';
  }

  printf("PERMISSIONS: %s\n", permissisons);

  if((owner = getpwuid(statBuffer.st_uid)) == NULL){
    printf("OWNER_NAME: %d\n", statBuffer.st_uid);
  } else {
    printf("OWNER_NAME: %s\n", owner -> pw_name);
  }

  if((group = getgrgid(statBuffer.st_gid)) == NULL){
    printf("GROUP_NAME: %d\n", statBuffer.st_gid);
  } else {
    printf("GROUP_NAME: %s\n", group -> gr_name);
  }

  printf("LAST_MODIFIED_ON: %s", ctime(&(statBuffer.st_mtime)));
  printf("LINK_COUNT: %d\n", statBuffer.st_nlink);

  if((S_ISCHR(statBuffer.st_mode))|| S_ISBLK(statBuffer.st_mode)) {
    printf("Major: %d, Minor %d\n", major(statBuffer.st_rdev), minor(statBuffer.st_rdev));
  } else {
    printf("SIZE (BYTES): %d\n", statBuffer.st_size);
    printf("INODE_NUMBER: %d \n\n", statBuffer.st_ino);
  }

}
