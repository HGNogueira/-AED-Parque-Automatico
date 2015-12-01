CC=gcc
TARGET=gestor
CFLAGS=-g -Wall -pedantic -ansi

gestor: gestor.o parkmap.o graphL.o LinkedList.o point.o prioQ.o htable.o
	$(CC) -o $(TARGET) $(CFLAGS) $^ 

gestor.o: gestor.c
	$(CC) -c gestor.c $(CFLAGS)

parkmap.o: parkmap.c parkmap.h
	$(CC) -c parkmap.c $(CFLAGS)

point.o: point.c point.h
	$(CC) -c point.c $(CFLAGS)

graphL.o: graphL.c graphL.h
	$(CC) -c graphL.c $(CFLAGS)

LinkedList.o: LinkedList.c LinkedList.h
	$(CC) -c LinkedList.c $(CFLAGS)

prioQ.o: prioQ.c prioQ.h
	$(CC) -c prioQ.c $(CFLAGS)

htable.o: htable.c htable.h LinkedList.h defs.h
	$(CC) -c htable.c $(CFLAGS)

clean:
	rm -f $(TARGET) *.o core.*
