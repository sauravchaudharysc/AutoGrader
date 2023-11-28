#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

int BUFFER_SIZE=1024;

int main(int argc, char *argv[])
{
 
    if (argc < 3) {
    	fprintf(stderr, "usage %s hostname port sourceCodeFile\n", argv[0]);
    	exit(0);
  	}

  //Buffer to Store Message 
  char buffer[BUFFER_SIZE];


  int sockfd, portno, n;

  //Structure for handling Server Internet Address
  struct sockaddr_in serv_addr;
  struct hostent *server; //To store returned value of gethostbyname

  //Command Line Input : Server IP and Port No 
  char* serverIP = strtok(argv[1], ":");
  char* serverPort = strtok(NULL, ":");
  
  portno = atoi(serverPort);

  /*Create socket*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
    perror("ERROR opening socket");
  
  /*Fill in Server Address*/
  /*If name is an IPv4 address, no lookup is performed and
    gethostbyname() simply copies name into the h_name field*/	
  server = gethostbyname(serverIP); 

  if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
  }
  bzero((char *)&serv_addr, sizeof(serv_addr)); // initialize serv_address bytes to all zeros
  serv_addr.sin_family = AF_INET; // Address Family of INTERNET
  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  serv_addr.sin_port = htons(portno);

  /*Connect To Server*/
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");

  printf("Server Connection Complete\n");
  
  char sourceCode[1024];
  bzero(sourceCode,1024);
  
  /*Loading The File*/
  FILE* sourceFile = fopen(argv[2], "r");
    if (sourceFile == NULL) {
        perror("Error opening source code file");
        close(sockfd);
        return 1;
    }
  fread(sourceCode, 1, sizeof(sourceCode), sourceFile);
  fclose(sourceFile);

  //Sending the File 
  n = write(sockfd,sourceCode,sizeof(sourceCode));
  if (n < 0) 
      perror("ERROR writing to socket");
  bzero(buffer,256);

  /*Read Reply From Server*/
  n = read(sockfd,buffer,255);
  if (n < 0) 
      perror("ERROR reading from socket");
  printf("%s",buffer);

  return 0;

}    




