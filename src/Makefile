CC = gcc
CFLAGS = -std=c99 -pedantic -Wall
DEBUG= -debug
LIB = -lev3dev-c

run: ../run
	clear
	sudo ../run $(DEBUG)

../run: main.c debug.c search.c
	$(CC) $(CFLAGS) $(LIB) -o ../run  main.c debug.c search.c

#../run: main.o debug.o search.o
	#$(CC) $(CFLAGS) $(LIB) -o run main.o debug.o search.o

#main.o: main.c
	#$(CC) $(CFLAGS) $(LIB) -c -o main.o  main.c

#debug.o: debug.c
	#$(CC) $(CFLAGS) $(LIB) -c -o debug.o  debug.c

#search.o: search.c
	#$(CC) $(CFLAGS) $(LIB) -c -o search.o  search.c