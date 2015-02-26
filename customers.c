/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* Personal includes */
#include "customers.h"
#include "customer_queue.h"

#define BUF_SIZE 80 /* Buffer size to read lines from input file */
#define BASE_TEN 10 /* Used in converting strings to ints */
#define SCALING_FACTOR 10000 /* Tenths of a second to microseconds */

/* Initializes a blank customer struct */
customer* initialize_customer(){
  customer *c = (customer*) malloc(sizeof(customer));
  c->num = 0;
  c->arrive = 0;
  c->service = 0;
  c->priority = 0;

  return c;
}

/* Parses a line from the input file and assigns the values to customer c */
/* According to the man pages, atoi has been deprecated and this use of
 * strtol is the recommended alternative */
void build_customer(customer *c, char *line){
  c->num = (int)strtol(strtok(line, ":"), (char**)NULL, BASE_TEN);
  c->arrive = (int)strtol(strtok(NULL, ","), (char**)NULL, BASE_TEN);
  c->service = (int)strtol(strtok(NULL, ","), (char**)NULL, BASE_TEN);
  c->priority = (int)strtol(strtok(NULL, "\n"), (char**)NULL, BASE_TEN);
}

/* Main control function executed by each customer thread */
void* thread_control(void *ptr){
  customer *c = (customer*) ptr;
  usleep(c->arrive * SCALING_FACTOR);
  printf("customer %2d arrives: arrival time (%.2d), service time (%.2d), "
         "priority (%2d). \n", c->num, c->arrive, c->service, c->priority);

  return (void*)0;
}

/* Process the input file provided from the command line */
void process_customers(FILE *fin){
  char buf[BUF_SIZE];
  int i,count;

  fgets(buf, BUF_SIZE, fin);
  count = (int)strtol(buf, (char**)NULL, BASE_TEN);
  pthread_t threads[count];

  for (i = 0; i < count; i++){
    fgets(buf, BUF_SIZE, fin);
    customer *c = initialize_customer();
    build_customer(c, buf);
    pthread_create(&threads[i], NULL, thread_control, (void*) c);
  }

  for (i = 0; i < count; i++) pthread_join(threads[i], NULL);
}
