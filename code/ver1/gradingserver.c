#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <fcntl.h>
#include<netinet/in.h>
#include<stdbool.h>


int BUFFER_SIZE=1024;

bool compileTimeError=false;
bool runTimeError=false;
const char* sourceFileName = "temp.c";
const char* executableFileName = "a";
const char* OutputFileName = "output.txt";

void makeCFile(const char* sourceCode){
    /*Storing the received lines of code in file*/
    //Open a file in write mode
    FILE* sourceFile = fopen(sourceFileName, "w");
    if (sourceFile == NULL) {
        perror("Error opening File..!!");
        return;
    }
    fwrite(sourceCode, 1, strlen(sourceCode), sourceFile);
    fclose(sourceFile);
}

void compileExecuteFile(){
    char compileCommand[256];
    int stderr_fd = dup(2);
    close(2);
    int err_fd = open("error.txt",O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU);
    sprintf(compileCommand, "gcc -o %s %s", executableFileName, sourceFileName);
    int compileResult = system(compileCommand);
    close(err_fd);
    dup2(stderr_fd,2);
    if(compileResult!=0){
    	compileTimeError=true;
    	return;
    }

    int stdout_fd = dup(1);
    stderr_fd = dup(2);
    close(2);
    err_fd = open("error.txt",O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU);
    close(1);
    int out_fd = open("output.txt",O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU);
    int execute = system("./a");
    //If we use \n it is writing the information in output.txt but all the information to be printed on client side is also printed on server side.
    //printf("Saurav\n");
    close(err_fd);
    close(out_fd);
    dup2(stdout_fd,1);
    dup2(stderr_fd,2);
    if(execute!=0){
        runTimeError=true;
        return;
    }
}


int main(int argc, char *argv[]){
	int sockfd,
	newsockfd,
	portno;

    if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
    }

    portno = atoi(argv[1]);

	/*Create Socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
		perror("Error Opening Socket");

	//Structure for handling Server Internet Address
	struct sockaddr_in serv_addr;
	bzero((char *)&serv_addr, sizeof(serv_addr)); // initialize serv_address bytes to all zeros
	serv_addr.sin_family = AF_INET; // Address Family of INTERNET
  	serv_addr.sin_addr.s_addr = INADDR_ANY;  //Any IP address. 
  	serv_addr.sin_port = htons(portno);

  	 /* bind socket to this port number on this machine */
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR on binding");
 
	 /* listen for incoming connection requests */
  	listen(sockfd, 1); // 1 means 1 connection requests can be in queue. 
  	//now server is listening for connections

  	//Structure for handling Client Internet Address
	struct sockaddr_in cli_addr;
	int clilen = sizeof(cli_addr);

	printf("Server Rolled\n");
     

    //Server Loop 
    while (1){ 
    compileTimeError=false;
    runTimeError=false;

	/* accept a new request, create a newsockfd */
     printf("Waiting For Client\n");
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     printf("Client Connected\n");
    sleep(3);
    if (newsockfd < 0) 
         perror("ERROR on accept");

     int n;
    
    /*Receiving Message From Client*/
    char buffer[1024]; //Buffer to store message received from client
    bzero(buffer, 1024); //set buffer to zero
    n = read(newsockfd,buffer,1024); //Read Message Sent From Client
    
    if (n < 0){
    	perror("ERROR reading from socket");
    }
    
    
    /*Performing Operations :-----Storing Message in Buffer*/
    makeCFile(buffer);
    compileExecuteFile();
    bzero(buffer, 1024); //set buffer to zero
    

    if(compileTimeError){
    	/*Loading The File*/
  		FILE* sourceFile = fopen("error.txt", "r");
    	if (sourceFile == NULL) {
        	perror("Error opening source code file");
        	close(sockfd);
        	return 1;
    	}
  		fread(buffer, 1, sizeof(buffer), sourceFile);
  		fclose(sourceFile);
    }else if(runTimeError){
    /*Loading The File*/
        FILE* sourceFile = fopen("error.txt", "r");
        if (sourceFile == NULL) {
            perror("Error opening source code file");
            close(sockfd);
            return 1;
        }
        fread(buffer, 1, sizeof(buffer), sourceFile);
        fclose(sourceFile);
    }else{
    	int stdout_fd = dup(1);	
    	close(1);
    	int out_fd = open("diff.txt",O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU);
        printf("OUTPUT ERROR\n");
    	int result = system("diff actual.txt output.txt");
	close(out_fd);
	dup2(stdout_fd,1);
	if(result==0){
	    	sprintf(buffer,"PASS\n");
        }else{
		/*Loading The File*/
  		FILE* sourceFile = fopen("diff.txt", "r");
    		if (sourceFile == NULL) {
    		  perror("Error opening source code file");
        	  close(sockfd);
        	  return 1;
    		}
  		fread(buffer, 1, sizeof(buffer), sourceFile);
  		fclose(sourceFile);
 	}
    }
    /*Message Stored :-- Operation Complete*/


    /*Sending Message To Client*/
    n = write(newsockfd,buffer,sizeof(buffer));
    printf("Client Disconnected\n");
    if (n < 0) perror("ERROR writing to socket");
    printf("\n");
  }
    return 0; 
}
