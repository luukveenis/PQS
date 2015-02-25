CC = gcc
CFLAGS = -c -ansi -Wall

all: pqs

pqs: pqs.o customers.o customer_queue.o
	$(CC) pqs.o customers.o customer_queue.o -o pqs

pqs.o: pqs.c
	$(CC) $(CFLAGS) pqs.c

customers.o: customers.c
	$(CC) $(CFLAGS) customers.c

customer_queue.o: customer_queue.c
	$(CC) $(CFLAGS) customer_queue.c

clean:
	rm -rf *.o pqs
