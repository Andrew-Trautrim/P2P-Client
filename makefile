CC = gcc
CFLAGS = -Wall -ansi

./bin/p2p : ./src/p2p.c
	$(CC) -o $@ ./src/p2p.c
