/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Personal includes */
#include "customers.h"
#include "customer_queue.h"

#define BUF_SIZE 80
#define BASE_TEN 10

customer* initialize_customer(){
  customer *c = (customer*) malloc(sizeof(customer));
  c->num = 0;
  c->arrive = 0;
  c->service = 0;
  c->priority = 0;

  return c;
}

void build_customer(customer *c, char *line){
  c->num = strtol(strtok(line, ":"), NULL, BASE_TEN);
  c->arrive = strtol(strtok(NULL, ","), NULL, BASE_TEN);
  c->service = strtol(strtok(NULL, ","), NULL, BASE_TEN);
  c->priority = strtol(strtok(NULL, "\n"), NULL, BASE_TEN);
}

void* thread_control(void *ptr){
  customer *c = (customer*) ptr;
  usleep(c->arrive * 10000);
  printf("customer %2d arrives: arrival time (%.2d), service time (%.2d), priority (%2d). \n", c->num, c->arrive, c->service, c->priority);

  return (void*)0;
}

void process_customers(FILE *fin){
  char buf[BUF_SIZE];
  int i,count;

  fgets(buf, BUF_SIZE, fin);
  count = strtol(buf, NULL, BASE_TEN);
  pthread_t threads[count];

  for (i = 0; i < count; i++){
    fgets(buf, BUF_SIZE, fin);
    customer *c = initialize_customer();
    build_customer(c, buf);
    pthread_create(&threads[i], NULL, thread_control, (void*) c);
  }

  for (i = 0; i < count; i++) pthread_join(threads[i], NULL);
}
