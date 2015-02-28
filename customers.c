#define _BSD_SOURCE

/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

/* Personal includes */
#include "customers.h"
#include "customer_queue.h"

#define BUF_SIZE 80 /* Buffer size to read lines from input file */
#define BASE_TEN 10 /* Used in converting strings to ints */
#define SCALING_FACTOR 100000 /* Tenths of a second to microseconds */
#define MICRO_FACTOR 1000000.0 /* Micro seconds = 10^6 seconds */

typedef enum { BUSY, IDLE } status_t;
typedef struct Clerk {
  status_t status;
  customer *serving;
  customer *next;
} clerk_t;

/* Global Variables */
pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
llist customer_queue = { NULL, NULL, 0 };
clerk_t clerk = { IDLE, NULL, NULL };
struct timeval start, current;

/* Initializes a blank customer struct */
customer* initialize_customer(){
  customer *c = (customer*) malloc(sizeof(customer));
  c->num = 0;
  c->arrive = 0;
  c->service = 0;
  c->priority = 0;
  c->print_wait = TRUE;

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
  if (clerk.status == IDLE && is_empty(&customer_queue)){
    clerk.status = BUSY;
    clerk.serving = c;
    pthread_mutex_unlock(&mutex1);
    return;
  }

  pthread_mutex_lock(&mutex2);
  enqueue(&customer_queue, c);
  clerk.next = customer_queue.head->cust;
  pthread_mutex_unlock(&mutex2);

  while (clerk.status == BUSY || clerk.next != c){
    if (c->print_wait){
      c->print_wait = FALSE;
      printf("Customer %2d waits for the finish of customer %2d.\n", c->num, clerk.serving->num);
    }
    pthread_cond_wait(&idle, &mutex1);
  }
  clerk.serving = c;
  clerk.status = BUSY;
  pthread_mutex_unlock(&mutex1);
  pthread_mutex_lock(&mutex2);
  delete_node(&customer_queue, customer_queue.head);
  clerk.next = customer_queue.head ? customer_queue.head->cust : NULL;
  pthread_mutex_unlock(&mutex2);
}

/* Sets the clerk to idle and wakes up customer threads */
void release_service(){
  pthread_mutex_lock(&mutex1);
  clerk.status = IDLE;
  pthread_mutex_unlock(&mutex1);
  pthread_cond_broadcast(&idle);
}

float get_time(){
  gettimeofday(&current, NULL);
  long int current_micros = (current.tv_sec * MICRO_FACTOR + current.tv_usec);
  long int start_micros = (start.tv_sec * MICRO_FACTOR + start.tv_usec);
  return ((current_micros - start_micros) / MICRO_FACTOR );
}

/* Main control function executed by each customer thread */
void* thread_control(void *ptr){
  customer *c = (customer*) ptr;
  usleep(c->arrive * SCALING_FACTOR);
  printf("Customer %2d arrives: arrival time (%.2f), service time (%.2f), "
         "priority (%2d). \n", c->num, get_time(), (c->service / 10.0), c->priority);
  request_service(c);
  printf("The clerk starts serving customer %2d at time %.2f.\n", clerk.serving->num, get_time());
  usleep(c->service * SCALING_FACTOR);
  printf("The clerk finishes the service to customer %2d at time %.2f.\n", clerk.serving->num, get_time());
  free(c);
  release_service();

  return (void*)0;
}

/* Process the input file provided from the command line */
void process_customers(FILE *fin){
  char buf[BUF_SIZE];
  int i,count;

  gettimeofday(&start, NULL);
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
