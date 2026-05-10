FLAGS = -g -Wall -lm -lsndfile -lportaudio
CC = gcc
PROG = clipped

all: $(PROG).c
	$(CC) $^ $(FLAGS) -o $(PROG)

clean:
	rm $(PROG) *.wav
