CC=gcc
TARGET=gestor
CFLAGS= -g -Wall -pedantic

gestor: gestor.o parkmap.o graphL.o LinkedList.o
	$(CC) -o $(TARGET) $^ $(CFLAGS)

gestor.o: gestor.c
	$(CC) -c gestor.c

parkmap.o: parkmap.c parkmap.h
	$(CC) -c parkmap.c

graphL.o: graphL.c graphL.h
	$(CC) -c graphL.c

LinkedList.o: LinkedList.c LinkedList.h
	$(CC) -c LinkedList.c

clean:
	rm -f $(TARGET) *.o core.*
