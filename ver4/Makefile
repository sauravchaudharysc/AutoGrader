all: client server

client: submit.c
		gcc -o client submit.c

server: ./Server/gradingMTServerQ.c server.o 
		gcc -o server2 ./Server/gradingMTServerQ.c server.o

server.o: ./Server/server.c ./Server/server.h 
		gcc -c ./Server/server.c

runserver: server
		./server 8080 10

clean:
		rm -f client server2
		rm -f ./dummy/*
		rm -f ./outputs/*
		rm -f *.o
