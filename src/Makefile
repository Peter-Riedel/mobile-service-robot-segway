CC     = gcc
CFLAGS = -std=c99 -pedantic -Wall -D _BSD_SOURCE
LIB    = -lev3dev-c
MAIN   = ../obj/main.o
DEBUG  = ../obj/debug.o
SEARCH = ../obj/search.o
DRIVE  = ../obj/drive.o
OBJ    = $(MAIN) $(DEBUG) $(SEARCH) $(DRIVE)
RUN    = ../run

run: $(RUN)
	clear
	sudo $(RUN) -debug

$(RUN): $(OBJ)
	$(CC) $(CFLAGS) $(LIB) -o $(RUN) $(OBJ)

$(MAIN): main.c
	$(CC) $(CFLAGS) $(LIB) -c -o $(MAIN) main.c

$(DEBUG): debug.c debug.h
	$(CC) $(CFLAGS) $(LIB) -c -o $(DEBUG) debug.c

$(SEARCH): search.c search.h
	$(CC) $(CFLAGS) $(LIB) -c -o $(SEARCH) search.c

$(DRIVE): drive.c drive.h
	$(CC) $(CFLAGS) $(LIB) -c -o $(DRIVE) drive.c
