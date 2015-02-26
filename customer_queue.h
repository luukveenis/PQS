#ifndef CUSTOMER_QUEUE_H_
#define CUSTOMER_QUEUE_H_

typedef struct {
  int num;
  int arrive;
  int service;
  int priority;
} customer;

struct node;
struct llist;

void delete_node(struct llist*, struct node*);
struct node* search(struct llist*, customer*);
void enqueue(struct llist*, customer*);

#endif
