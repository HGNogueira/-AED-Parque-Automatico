CC=gcc
TARGET=autopark
CFLAGS=-O3 -Wall -pedantic -ansi

autopark: gestor.o parkmap.o graphL.o LinkedList.o point.o prioQ.o htable.o escreve_saida.o queue.o
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

escreve_saida.o: escreve_saida.h
	$(CC) -c escreve_saida.c $(CFLAGS)

queue.o: queue.h queue.c
	$(CC) -c queue.c $(CFLAGS)

clean:
	rm -f $(TARGET) *.o core.*
