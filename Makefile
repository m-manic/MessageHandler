CC=g++
CFLAGS=-I. 

message_handler: MessageHandler.cpp MessageHandler.h main.cpp
	$(CC) -pthread -std=c++11 -o message_handler MessageHandler.cpp main.cpp $(CFLAGS)