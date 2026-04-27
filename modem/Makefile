FLAGS = -g -Wall -lm -lsndfile
CC = gcc
PROG = modem

all: $(PROG).c
	$(CC) $^ $(FLAGS) -o $(PROG)

clean:
	rm $(PROG)
