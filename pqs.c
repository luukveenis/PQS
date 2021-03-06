/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Personal includes */
#include "customers.h"

int main (int argc, char **argv){
  FILE *fin;

  if (argc <= 1 || argc > 2){
    printf("You must specify exactly ONE input file!\n");
    exit(EXIT_FAILURE);
  }
  fin = fopen(argv[1], "r");
  process_customers(fin);
  fclose(fin);

  return 0;
}
