all: client server

client: submit.c
		gcc -o submit submit.c

server: ./Server/gradingMTServerQ.c ./Server/server.c 
		gcc -o server ./Server/gradingMTServerQ.c ./Server/server.c

runserver: server
		./server 8080 10

clean:
		rm -f submit server
		rm -f ./Error/*
		rm -f ./CPP/*
		rm -f ./data/*
		rm -f ./Output/*
		rm -f *.o
