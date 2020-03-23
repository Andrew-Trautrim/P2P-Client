CC = g++
CFLAGS = -Iinclude -pthread
SRC = ./src
INC = ./include
BIN = ./bin

p2p : $(SRC)/p2p.cpp $(BIN)/p2p_network.o
	$(CC) $(SRC)/p2p.cpp $(BIN)/p2p_network.o $(CFLAGS) -o $@ 

$(BIN)/p2p_network.o : $(SRC)/p2p_network.cpp
	$(CC) $(SRC)/p2p_network.cpp $(CFLAGS) -c -o $@

clean :
	rm $(BIN)/*.o
