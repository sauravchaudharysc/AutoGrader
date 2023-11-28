all : server client 

server : gradingserver.c
	gcc -o server gradingserver.c

client : gradingclient.c
	gcc -o client gradingclient.c
	
clean :
	-rm -f diff.txt error.txt output.txt temp.c a client server

