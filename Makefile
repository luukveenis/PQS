CC = gcc
CFLAGS = -c -ansi -Wall

all: pqs

pqs: pqs.o customers.o
	$(CC) pqs.o customers.o -o pqs

pqs.o: pqs.c
	$(CC) $(CFLAGS) pqs.c

customers.o: customers.c
	$(CC) $(CFLAGS) customers.c

clean:
	rm -rf *.o pqs
