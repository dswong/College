#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int authenticate(char* password) {
  char buf[80]; // 8 character password
  char tmp[9]; // same as above
  FILE *fp;

  fp = fopen("/home/mr177/bin/password.txt" , "r");

  fscanf(fp, "%s\n", tmp);
  fclose(fp);
  strcpy(buf, password);
  
  return strcmp(password, tmp);
}


int main(int argc, char *argv[]) {
  char tmp[32];
  FILE *fp;

  if(argc != 2) {
    printf("Usage: %s password (password must be 8 characters or less\n", argv[0]);
  } else if(authenticate(argv[1]) == 0) {
    printf("Success!\n");
    fp = fopen("/home/mr177/bin/secret.txt" , "r");
    fscanf(fp, "%s\n", tmp);
    printf("The secret word is: %s\n", tmp);
    fclose(fp);
  } else {
    printf("Access Denied!\n");
  }
  exit(0);
}
