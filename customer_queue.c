#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer_queue.h"

/* Convenience function to test emptiness */
int is_empty(llist_ref list){
  return list->size == 0 ? 1 : 0;
}

/* Creates a new node
 * Both back and prev pointers will be NULL */
node_ptr create_node(customer *c)
{
  /* Create the node */
  node_ptr new_node = malloc(sizeof(node));
  new_node->next = NULL;
  new_node->prev = NULL;
  new_node->cust = c;
  return new_node;
}

/* Destroys the given node, freeing the memory
 * All pointers are set to NULL */
void deallocate_node(node_ptr node)
{
  node->next = NULL;
  node->prev = NULL;
  node->cust = NULL;
  free(node);
}

/* Deletes the given node */
void delete_node(llist_ref list, node_ptr node)
{
  /* Case 1: It is the only node in the list */
  if (NULL == node->prev && NULL == node->next)
  {
    list->head = NULL;
    list->tail = NULL;
    deallocate_node(node);
  }
  /* Case 2: It is the head of the list */
  else if (NULL == node->prev && NULL != node->next)
  {
    list->head = node->next;
    node->next->prev = NULL;
    deallocate_node(node);
  }
  /* Case 3: It is the tail of the list */
  else if (NULL != node->prev && NULL == node->next)
  {
    list->tail = node->prev;
    node->prev->next = NULL;
    deallocate_node(node);
  }
  /* Case 4: It is in the middle of the list */
  else
  {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    deallocate_node(node);
  }
  list->size--;
}

/* Searches for and returns the node corresponding to customer */
node_ptr search(llist_ref list, customer *c)
{
  node_ptr current = list->head;
  while (NULL != current)
  {
    if (current->cust == c)
    {
      return current;
    }
    current = current->next;
  }
  return current;
}

/* Compares two customers:
 * Result is greater than 0 if c1 is higher priority than c2
 * Result is less than 0 otherwise */
int customer_cmp(customer *c1, customer *c2){
  if (c1->priority > c2->priority){
    return 1;
  } else if (c1->priority == c2->priority){
    if (c1->arrive < c2->arrive){
      return 1;
    } else if (c1->arrive == c2->arrive){
      if (c1->service < c2->service){
        return 1;
      } else if (c1->service == c2->service){
        return c1->num < c2->num ? 1 : -1;
      }
    }
  }
  return -1;
}

/* Inserts a node with the given customer at the head of the list */
void insert_at_head(llist_ref list, node_ptr new_node)
{
  if (is_empty(list))
  {
    list->head = new_node;
    list->tail = new_node;
  }
  else
  {
    list->head->prev = new_node;
    new_node->next = list->head;
    list->head = new_node;
  }
  list->size++;
}

/* Inserts a node with the given customer at the tail of the list */
void insert_at_tail(llist_ref list, node_ptr new_node)
{
  if (is_empty(list))
  {
    list->head = new_node;
    list->tail = new_node;
  }
  else
  {
    list->tail->next = new_node;
    new_node->prev = list->tail;
    list->tail = new_node;
  }
  list->size++;
}

/* Inserts node new before node old in the list */
void insert_before(llist_ref list, node_ptr new, node_ptr old)
{
  new->next = old;
  new->prev = old->prev;
  old->prev = new;
  if (new->prev == NULL){
    list->head = new;
  } else {
    new->prev->next = new;
  }
  list->size++;
}

/* Insert a customer into the list keeping it sorted by priority */
void enqueue(llist_ref list, customer *c){
  node_ptr new_node = create_node(c);
  node_ptr current;

  if (list->size == 0){
    insert_at_head(list, new_node);
  } else {
    for (current = list->head; current; current = current->next){
      if (customer_cmp(c, current->cust) > 0){
        insert_before(list, new_node, current);
        break;
      }
    }
    /* Add it to the end if it should be last */
    if (current == NULL) insert_at_tail(list, new_node);
  }
}

/* Simply prints all the elements in the list */
/* Primarily to assist with development */
void print_list(llist_ref list)
{
  int counter = 1;
  node_ptr current = list->head;
  while(NULL != current)
  {
    printf("Node %d: Customer %d\n", counter, current->cust->num);
    counter++;
    current = current->next;
  }
}
