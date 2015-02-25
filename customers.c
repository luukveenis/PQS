#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customers.h"

#define BUF_SIZE 80

customer* initialize_customer(){
  customer *c = (customer*) malloc(sizeof(customer));
  c->num = 0;
  c->arrive = 0;
  c->service = 0;
  c->priority = 0;

  return c;
}

void build_customer(customer *c, char *line){
  c->num = atoi(strtok(line, ":"));
  c->arrive = atoi(strtok(NULL, ","));
  c->service = atoi(strtok(NULL, ","));
  c->priority = atoi(strtok(NULL, "\n"));
}

void process_customers(FILE *fin){
  char buf[BUF_SIZE];
  int i,count;

  fgets(buf, BUF_SIZE, fin);
  for (i = 0, count = atoi(buf) ; i < count; i++){
    fgets(buf, BUF_SIZE, fin);
    customer *c = initialize_customer();
    build_customer(c, buf);
  }
}
