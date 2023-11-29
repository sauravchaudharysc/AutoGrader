/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include "server.h"
int front = 0, rear = 0, taskCount = 0, queue[20];
bool done=false;
pthread_mutex_t queueMutex;
pthread_cond_t taskReady;

pthread_mutex_t request_file_lock;



void queue_insert(int item)
{
    rear = (rear + 1) % QUEUE_SIZE;
    if (front == rear)
    {
        printf("\nOverflow\n");
        if (rear == 0)
            rear = QUEUE_SIZE - 1;
        else
            rear--;
    }
    else
    {
        queue[rear] = item;
    }
}

int queue_delete()
{
    int item;
    if (front == rear)
    {
        printf("\nThe Queue is empty\n");
    }
    else
    {
        front = (front + 1) % QUEUE_SIZE;
        item = queue[front];
        return item;
    }
}


void insertion(int newsockfd){
int id = generate_request_id();
char* sourceFileName = makeProgramFileName(id);
int reqType=recv_file(newsockfd,sourceFileName);
if (reqType!= 0){
     close(newsockfd);
     return;  
}
queue_insert(id);
taskCount++;
char reqID[20];
sprintf(reqID, "%d", id);

pthread_mutex_lock(&request_file_lock);

/***Insertion In Request File ***/
FILE *file = fopen("data/request_file.txt", "a");
if (file == NULL) {
     printf("Failed to open the file for writing.\n");          
     pthread_mutex_unlock(&request_file_lock);
     return ;
}

time_t timestamp;
timestamp = time(NULL);
char time[20];
sprintf(time,"%ld",timestamp);
fprintf(file, "%-13d%-14s%-11s%-14s",id, time, "In_Queue", "Under_Process\n");
fclose(file);
pthread_mutex_unlock(&request_file_lock);

char response[1024];
sprintf(response,"I got your code file for grading. Your Request Id is %d\n",id);
int n = send(newsockfd, response, sizeof(response), 0);
close(newsockfd);
return;
}


void *grader(void * arg) { 
 while(1){
 int newsockfd,n;
 bool found=false;
 pthread_mutex_lock(&queueMutex);
	 while(taskCount==0 && !done){
		pthread_cond_wait(&taskReady,&queueMutex);
	}

	if(taskCount>0){
	    found = true;
            newsockfd = queue_delete();
            taskCount--;
            pthread_cond_signal(&taskReady);
	}
 pthread_mutex_unlock(&queueMutex);
 
   if(found==true){
         int id = newsockfd;
	 char* sourceFileName = makeProgramFileName(id);
	 char* execFileName = makeExecFileName(id);
	 char* compileCommand = compile_command(id,sourceFileName,execFileName);
	 char* execCommand = run_command(id,execFileName);
	 char buffi[200];
	 	 if (system(compileCommand) !=0 ) {
	 	 	sprintf(buffi,"COMPILER_ERROR\n");
//		    	n = send(newsockfd, "COMPILER ERROR\n", 16, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
	 	  } //here if no compiler error
	  //Run the executable, send back 'RUNTIME ERROR' message to client  if runtime error
	  //Write runtime error message to a local file runtime_err.txt, program output to a file out.txt
	  //NO NEED TO SEND RUNTIME ERROR MESSAGE BACK TO CLIENT. 
	  
	 	   else if (system(execCommand) !=0 ) { 						
	 	 	sprintf(buffi,"RUN_TIME_ERROR\n");		   
		    	//n = send(newsockfd, "RUNTIME ERROR\n", 15, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
	 	   }  //here if no runtime error
	  //Write a message "PROGRAM RAN" to client 
	  //NO NEED TO SEND PROGRAM OUTPUT BACK TO CLIENT. 
		   else { 		 
	 	 	sprintf(buffi,"SUCESSFUL\n");	 		
	 		//n = send(newsockfd, "PROGRAM RAN\n", 13, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
		    }
	    //printf("Req ID: %d\n",id);
	    change_status_reqfile(id,"Done",buffi,request_file_lock);
	    if (n < 0)
	      error("ERROR writing to socket");
	    free(execFileName);
	    free(execCommand);
	    free(compileCommand);
	    free(sourceFileName);
 }
 /*
 if (done == true && taskCount == 0){
            break;
        }*/
 }   
}    


int main(int argc, char *argv[]) {

  int sockfd, //the listen socket descriptor (half-socket)
   newsockfd, //the full socket after the client connection is made
   portno; //port number at which server listens

  socklen_t clilen; //a type of an integer for holding length of the socket address
  char buffer[256]; //buffer for reading and writing the messages
  struct sockaddr_in serv_addr, cli_addr; //structure for holding IP addresses
  int n;

  if (argc < 3) {
    fprintf(stderr, "Usage..!!, <port> <threads>\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  //AF_INET means Address Family of INTERNET. SOCK_STREAM creates TCP socket (as opposed to UDP socket)
 // This is just a holder right now, note no port number either. It needs a 'bind' call


  if (sockfd < 0)
    error("ERROR opening socket");

 
  bzero((char *)&serv_addr, sizeof(serv_addr)); // initialize serv_address bytes to all zeros
  
  serv_addr.sin_family = AF_INET; // Address Family of INTERNET
  serv_addr.sin_addr.s_addr = INADDR_ANY;  //Any IP address. 

//Port number is the first argument of the server command
  portno = atoi(argv[1]);
  serv_addr.sin_port = htons(portno);  // Need to convert number from host order to network order

  /* bind the socket created earlier to this port number on this machine 
 First argument is the socket descriptor, second is the address structure (including port number).
 Third argument is size of the second argument */
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 1); // 1 means 1 connection requests can be in queue. 
  //now server is listening for connections


  clilen = sizeof(cli_addr);  //length of struct sockaddr_in


  /* accept a new request, now the socket is complete.
  Create a newsockfd for this socket.
  First argument is the 'listen' socket, second is the argument 
  in which the client address will be held, third is length of second
  */
  int reqID = 0;
  
  if(pthread_mutex_init(&queueMutex,NULL)!=0){
	printf("Mutex init failed\n");
  }
  
  if(pthread_cond_init(&taskReady,NULL)!=0){
	printf("Cond init failed\n");
  }
  
  pthread_t p[10];
  for (int i = 0; i < 10; i++){
        if (pthread_create(&p[i], NULL, grader, NULL) != 0)
        {
            printf("ERROR: Could not create thread %d", i);
            exit(EXIT_FAILURE);
        }
  }
	  
  while (1){
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    pthread_mutex_lock(&queueMutex);
    while ((rear + 1) % QUEUE_SIZE == front){
        pthread_cond_wait(&taskReady, &queueMutex);
    }
    insertion(newsockfd);
    pthread_cond_signal(&taskReady);
    pthread_mutex_unlock(&queueMutex);
  }
    
  return 0;
}



