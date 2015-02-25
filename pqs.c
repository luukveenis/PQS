#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_customers(FILE *fin){
  char buf[100];
  int len;

  while (fgets(buf, 100, fin)){
    len = strlen(buf);
    if (buf[len-1] == '\n') buf[len-1] = '\0';
    printf("%s\n", buf);
  }
}

int main (int argc, char **argv){
  FILE *fin;
  int i;

  if (argc <= 1 || argc > 2){
    printf("You must specify exactly ONE input file!\n");
    exit(EXIT_FAILURE);
  }
  fin = fopen(argv[1], "r");
  process_customers(fin);

  return 0;
}
