
CC = gcc

.PHONY: all  server client 

all: server client
	make server
	make client
	
server:
	$(CC) server.c -g -o server -lpthread list.c
	./server localhost 8080

client:
	$(CC) client.c -g -o client -lpthread
