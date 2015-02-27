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
#define SCALING_FACTOR 100000 /* Tenths of a second to microseconds */

/* Global Variables */
pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
llist customer_queue = { NULL, NULL, 0};
int clerk_idle = 1;

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

/* Function that controls which customer is served next */
void request_service(customer *c){
  pthread_mutex_lock(&mutex1);
  if (clerk_idle && is_empty(&customer_queue)){
    clerk_idle = 0;
    pthread_mutex_unlock(&mutex1);
    return;
  }

  pthread_mutex_lock(&mutex2);
  enqueue(&customer_queue, c);
  pthread_mutex_unlock(&mutex2);

  while (!clerk_idle || customer_queue.head->cust != c){
    printf("Customer %d waiting\n", c->num);
    pthread_cond_wait(&idle, &mutex1);
  }

  pthread_mutex_lock(&mutex2);
  delete_node(&customer_queue, customer_queue.head);
  pthread_mutex_unlock(&mutex2);
}

/* Sets the clerk to idle and wakes up customer threads */
void release_service(){
  clerk_idle = 1;
  pthread_mutex_unlock(&mutex1);
  pthread_cond_broadcast(&idle);
}

/* Main control function executed by each customer thread */
void* thread_control(void *ptr){
  customer *c = (customer*) ptr;
  usleep(c->arrive * SCALING_FACTOR);
  printf("Customer %2d arrives: arrival time (%.2d), service time (%.2d), "
         "priority (%2d). \n", c->num, c->arrive, c->service, c->priority);
  request_service(c);
  printf("Serving customer %d\n", c->num);
  usleep(c->service * SCALING_FACTOR);
  release_service();
  /* printf("Customer %d should have just released service.\n", c->num); */

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
