#ifndef CUSTOMER_QUEUE_H_
#define CUSTOMER_QUEUE_H_

typedef enum { FALSE = 0, TRUE = 1 } boolean;
typedef struct {
  int num;
  int arrive;
  int service;
  int priority;
  boolean print_wait;
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

int is_empty(llist_ref);
llist_ref initialize_list();
void delete_node(llist_ref, node_ptr);
node_ptr search(llist_ref, customer*);
void enqueue(llist_ref, customer*);
void print_list(llist_ref);

#endif
