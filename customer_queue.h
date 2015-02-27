#ifndef CUSTOMER_QUEUE_H_
#define CUSTOMER_QUEUE_H_

typedef struct {
  int num;
  int arrive;
  int service;
  int priority;
} customer;

typedef struct node* node_ptr;
typedef struct node {
  node_ptr next;
  node_ptr prev;
  customer *cust;
} node;

typedef struct llist* llist_ref;
typedef struct llist {
  node_ptr head;
  node_ptr tail;
  int size;
} llist;

void delete_node(struct llist*, struct node*);
struct node* search(struct llist*, customer*);
void enqueue(struct llist*, customer*);

#endif
