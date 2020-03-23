CC = g++
CFLAGS = -Iinclude -pthread

./bin/p2p: ./src/p2p.cpp ./src/p2p_network.cpp
	$(CC) ./src/p2p.cpp $(CFLAGS) -o $@ 

clean :
	rm ./bin/*.o ./bin/sesson
