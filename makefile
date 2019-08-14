CC = gcc
CFLAGS = -Iincludes

./bin/session: ./bin/session.o ./bin/p2p.o ./bin/p2p_chat.o
	$(CC) -o $@ ./bin/session.o ./bin/p2p.o ./bin/p2p_chat.o -lpthread

./bin/p2p.o : ./includes/p2p.h ./src/p2p.c
	$(CC) $(CFLAGS) -o $@ -c ./src/p2p.c

./bin/p2p_chat.o : ./includes/p2p_chat.h ./src/p2p_chat.c
	$(CC) $(CFLAGS) -o $@ -c ./src/p2p_chat.c

./bin/session.o : ./includes/p2p.h ./src/session.c
	$(CC) $(CFLAGS) -o $@ -c ./src/session.c

clean :
	rm ./bin/p2p.o ./bin/session.o
