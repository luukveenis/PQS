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

typedef enum { BUSY, IDLE } status;

/* Global Variables */
pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
llist customer_queue = { NULL, NULL, 0};
status clerk_status = IDLE;
int serving = -1;
int next = -1;

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
  if (clerk_status == IDLE && is_empty(&customer_queue)){
    clerk_status = BUSY;
    serving = c->num;
    pthread_mutex_unlock(&mutex1);
    return;
  }

  pthread_mutex_lock(&mutex2);
  enqueue(&customer_queue, c);
  next = customer_queue.head->cust->num;
  pthread_mutex_unlock(&mutex2);

  while (clerk_status == BUSY || next != c->num){
    printf("Customer %2d waits for the finish of customer %2d. \n", c->num, serving);
    pthread_cond_wait(&idle, &mutex1);
  }
  serving = c->num;
  pthread_mutex_lock(&mutex2);
  delete_node(&customer_queue, customer_queue.head);
  next = customer_queue.head ? customer_queue.head->cust->num : -1;
  pthread_mutex_unlock(&mutex2);
}

/* Sets the clerk to idle and wakes up customer threads */
void release_service(){
  clerk_status = IDLE;
  pthread_mutex_unlock(&mutex1);
  pthread_cond_broadcast(&idle);
}

/* Main control function executed by each customer thread */
void* thread_control(void *ptr){
  customer *c = (customer*) ptr;
  usleep(c->arrive * SCALING_FACTOR);
  printf("Customer %2d arrives: arrival time (%.2f), service time (%.2f), "
         "priority (%2d). \n", c->num, (c->arrive / 10.0), (c->service / 10.0), c->priority);
  request_service(c);
  printf("Serving customer %d\n", c->num);
  usleep(c->service * SCALING_FACTOR);
  release_service();

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
