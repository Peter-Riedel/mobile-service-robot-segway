CC = gcc
CFLAGS = -std=c99 -pedantic -Wall
DEBUG= -debug
LIB = -lev3dev-c

run: ../run
	clear
	sudo ../run $(DEBUG)

../run: main.c debug.c search.c
	$(CC) $(CFLAGS) $(LIB) -o ../run  main.c debug.c search.c
