CC = gcc
CFLAGS = -Iincludes

./bin/session: ./bin/session.o ./bin/p2p.o
	$(CC) -o $@ ./bin/session.o ./bin/p2p.o

./bin/p2p.o : ./includes/p2p.h ./src/p2p.c
	$(CC) $(CFLAGS) -o $@ -c ./src/p2p.c

./bin/session.o : ./includes/p2p.h ./src/session.c
	$(CC) $(CFLAGS) -o $@ -c ./src/session.c

clean :
	rm ./bin/p2p.o ./bin/session.o
