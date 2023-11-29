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
#include<sys/time.h>
#include <sys/resource.h>


const int BUFFER_SIZE = 1024;
const int MAX_FILE_SIZE_BYTES = 4;


#define QUEUE_SIZE 20
int front = 0, rear = 0, taskCount = 0, queue[QUEUE_SIZE];
bool done=false;
pthread_mutex_t queueMutex;
pthread_cond_t taskReady;

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




void error(char *msg) {
  perror(msg);
  exit(1);
}

//Utility Function to receive a file of any size to the grading server
int recv_file(int sockfd, char* file_path)
//Arguments: socket fd, file name (can include path) into which we will store the received file
{
    char buffer[BUFFER_SIZE]; //buffer into which we read  the received file chars
    bzero(buffer, BUFFER_SIZE); //initialize buffer to all NULLs
    FILE *file = fopen(file_path, "wb");  //Get a file descriptor for writing received data into file
    if (!file)
    {
        perror("Error opening file");
        return -1;
    }

	
	//buffer for getting file size as bytes
    char file_size_bytes[MAX_FILE_SIZE_BYTES];
    //first receive  file size bytes
    if (recv(sockfd, file_size_bytes, sizeof(file_size_bytes), 0) == -1)
    {
        perror("Error receiving file size");
        fclose(file);
        return -1;
    }
   
    int file_size;
    //copy bytes received into the file size integer variable
    memcpy(&file_size, file_size_bytes, sizeof(file_size_bytes));
    
    //some local printing for debugging
    printf("File size is: %d\n", file_size);
    
    //now start receiving file data
    size_t bytes_read = 0, total_bytes_read =0;;
    while (true)
    {
    	  //read max BUFFER_SIZE amount of file data
        bytes_read = recv(sockfd, buffer, BUFFER_SIZE, 0);

        //total number of bytes read so far
        total_bytes_read += bytes_read;

        if (bytes_read <= 0)
        {
            perror("Error receiving file data");
            fclose(file);
            return -1;
        }

		//write the buffer to the file
        fwrite(buffer, 1, bytes_read, file);

	// reset buffer
        bzero(buffer, BUFFER_SIZE);
        
       //break out of the reading loop if read file_size number of bytes
        if (total_bytes_read >= file_size)
            break;
    }
    fclose(file);
    return 0;
}

/*Utility Function that takes some number as an ID, and source code file name (programFile) and executable file name (execFile) as arguments, and creates a compile command such that executable file gets the name in execFile. and compiler error output if any goes into a file compiler_err<id>.txt

E.g. if the call is :

comp_comm = compile_command (2123, file2123.cpp, prog2123)

then it till return the string (comp_comm will get this string):

"gcc -o prog2123 file2123.cpp 2> compiler_err2123.txt"

YOU SHOULD NOT TRY TO CHANGE THIS FUNCTION. JUST USE IT AS IS.

*/
char* compile_command(int id, char* programFile, char* execFile) {

  char *s;
  char s1[20];
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));
  strcpy(s, "g++ -o ");
  strcat(s, execFile);
  strcat(s, "  ");
  strcat(s, programFile);
  strcat(s, " 2> compiler_err");
 	sprintf(s1, "%d", id);	
 	strcat(s, s1);	
  strcat(s, ".txt");
  printf("%s\n",s);
  return s;
}
    
/*Utility Function that takes some number as an ID, and executable file name (execFile) as arguments, and creates a run command such that output file gets a name such as out<id>.txt. and runtime error output if any goes into a file runtime_err<id>.txt

E.g. if the call is :

run_comm = run_command (2123, prog2123)

then it till return the string (run_comm will get this string):

"./prog2123 > out2123.txt 2> runtime_err2123.txt"

YOU SHOULD NOT TRY TO CHANGE THIS FUNCTION. JUST USE IT AS IS.

*/

char* run_command(int id, char* execFileName) {

  char *s;
  char s1[20];
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));
 	sprintf(s1, "%d", id);	  

  strcpy(s, "./");
  strcat(s, execFileName);
  strcat(s, " > out");
 	strcat(s, s1);	
  strcat(s, ".txt");
  strcat(s, " 2> runtime_err");
 	strcat(s, s1);	
 	strcat(s, ".txt");	
  printf("%s\n",s);
  return s;
}

/* Utility function that given a number as an argument returns a  source code file name of the format file<id>.cpp

e.g. if the call is

fname = makeProgramFileName (2123);

then fname will get set to 'file2123.cpp'

YOU SHOULD NOT TRY TO CHANGE THIS FUNCTION. JUST USE IT AS IS.
*/

char *makeProgramFileName(int id) {

  char *s;
  char s1[20];	
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));

  sprintf(s1, "%d", id);	  
  strcpy (s, "file");
  strcat (s, s1);
  strcat (s, ".cpp");
  return s;
}  
  
/* Utility function that given a number as an argument returns an executable file name of the format prog<id>

e.g. if the call is

execName = makeExecFileName (2123);

then execName will get set to 'prog2123'

YOU SHOULD NOT TRY TO CHANGE THIS FUNCTION. JUST USE IT AS IS.
*/
char *makeExecFileName(int id) {

  char *s;
  char s1[20];	
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));
  sprintf(s1, "%d", id);	  
  strcpy (s, "prog");
  strcat (s, s1);
  return s;
} 

/* The following should be your worker thread. Fill in the blank (marked ____) and also the whole function code to complete it. */

void *grader(void * arg) { // ***FIB

/* ***FIB, all of your main grading code will go here. 
 Just copy-paste the relevant part from gradingSTserver.c and make relevant changes. Especially add and modify  parts related to creating unique file names. All the utility functions are given to you above. Study the usage given above of the functions (read comments above the functions) BEFORE WRITING ANY CODE HERE. The relevant functions are:
 
 makeProgramFileName, makeExecFileName, compile_command, run_command. 
 
 The availability of these functions should make your coding super-trivial.
 
See codefragments.txt for a hint of what you can use to get a unique number. 

Finally, all the strings returned by the utility functions have been allocated on the heap. Take care to 'free' them before the thread function exits.
*/
 while(1){
 int newsockfd;
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
         struct rusage usage;
	 getrusage(RUSAGE_SELF, &usage);
         int id = (int)pthread_self();
	 char* sourceFileName = makeProgramFileName(id);
	 if (recv_file(newsockfd,sourceFileName) != 0){
		 close(newsockfd);
		 return 0;
	 }
	 
	 int n = send(newsockfd, "I got your code file for grading\n", 33, 0);
	 
	 char* execFileName = makeExecFileName(id);
	 
	 char* compileCommand = compile_command(id,sourceFileName,execFileName);
	 char* execCommand = run_command(id,execFileName);
	 double elapsed_time;
	 struct timeval start, end;
	 gettimeofday(&start, NULL);
	 	 if (system(compileCommand) !=0 ) {
		 	gettimeofday(&end, NULL);
		 	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
		 	printf("%f\n",elapsed_time);
		 	if(elapsed_time>50)
		 	{
		 		printf("TIME LIMIE EXCEEDED!");
		 		n = send(newsockfd, "TLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}
		 	if(usage.ru_maxrss>3000)
		 	{
		 		printf("MEMORY LIMIT EXCEED!");
		 		n = send(newsockfd, "MLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}	
		    	n = send(newsockfd, "COMPILER ERROR\n", 16, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
	 	  } //here if no compiler error
	  //Run the executable, send back 'RUNTIME ERROR' message to client  if runtime error
	  //Write runtime error message to a local file runtime_err.txt, program output to a file out.txt
	  //NO NEED TO SEND RUNTIME ERROR MESSAGE BACK TO CLIENT. 
	  
	 	   else if (system(execCommand) !=0 ) { 
	 	   	gettimeofday(&end, NULL);
		 	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
		 	if(elapsed_time>50)
		 	{
		 		printf("TIME LIMIE EXCEEDED!");
		 		n = send(newsockfd, "TLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}
		 	if(usage.ru_maxrss>3000)
		 	{
		 		printf("MEMORY LIMIT EXCEED!");
		 		n = send(newsockfd, "MLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}						
		    	n = send(newsockfd, "RUNTIME ERROR\n", 15, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
	 	   }  //here if no runtime error
	  //Write a message "PROGRAM RAN" to client 
	  //NO NEED TO SEND PROGRAM OUTPUT BACK TO CLIENT. 
		   else { 
		   	gettimeofday(&end, NULL);
		 	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
		 	if(elapsed_time>50)
		 	{
		 		printf("TIME LIMIE EXCEEDED!");
		 		n = send(newsockfd, "TLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}
		 	if(usage.ru_maxrss>3000)
		 	{
		 		printf("MEMORY LIMIT EXCEED!");
		 		n = send(newsockfd, "MLE!\n", 16, 0);
		 		close(newsockfd);
		 		return NULL;
		 	}		 
	 		n = send(newsockfd, "PROGRAM RAN\n", 13, 0);
		    	if (n < 0)
			      error("ERROR writing to socket");	
	 		    close(newsockfd);	
		    }
	    gettimeofday(&end, NULL);
	    
	    close(newsockfd);
	    if (n < 0)
	      error("ERROR writing to socket");
	    free(execFileName);
	    free(execCommand);
	    free(compileCommand);
	    free(sourceFileName);
	    printf("Memory usage: %ld kilobytes\n", usage.ru_maxrss);	
 }
 /*
 if (done == true && taskCount == 0){
            break;
        }*/
 }   
}    


int main(int argc, char *argv[]) {


/*You should not need to change any code FROM HERE (till the line marked TILL HERE)*/

  int sockfd, //the listen socket descriptor (half-socket)
   newsockfd, //the full socket after the client connection is made
   portno; //port number at which server listens

  socklen_t clilen; //a type of an integer for holding length of the socket address
  char buffer[256]; //buffer for reading and writing the messages
  struct sockaddr_in serv_addr, cli_addr; //structure for holding IP addresses
  int n;

  if (argc < 3) {
    fprintf(stderr, "ERROR, <Port> <Threads>\n");
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
    queue_insert(newsockfd);
    taskCount++;
    pthread_cond_signal(&taskReady);
    pthread_mutex_unlock(&queueMutex);
  }
    
  return 0;
}

