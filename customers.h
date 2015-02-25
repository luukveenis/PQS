#ifndef CUSTOMERS_H_
#define CUSTOMERS_H_

typedef struct {
  int num;
  int arrive;
  int service;
  int priority;
} customer;

void process_customers(FILE*);

#endif
