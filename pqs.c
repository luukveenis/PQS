#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv){
  FILE *fin;
  int i;

  if (argc <= 1 || argc > 2){
    printf("You must specify exactly ONE input file!\n");
    exit(EXIT_FAILURE);
  }
  fin = fopen(argv[0], "r");

  return 0;
}
