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


int generate_request_id() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long seed = tv.tv_sec * 1000000 + tv.tv_usec;
    srand(seed);
    return rand()%10000;
}

// Function to search for a request ID and check its status
int searchRequestID(int targetID, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return an error code
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        struct LogEntry entry;
        if (sscanf(line, "%d %d %s %s", &entry.ID, &entry.TimeStamp, entry.Status, entry.Result) == 4) {	
            //printf("%d %d %s %s\n", entry.ID, entry.TimeStamp, entry.Status, entry.Result);
            if (entry.ID == targetID) {
                if(strcmp(entry.Status,"Done")==0){
                	if(strcmp(entry.Result,"SUCESSFUL")==0){
                		return 2;
                	}else if(strcmp(entry.Result,"COMPILE")==0){
                		return 3;
                	}else{
                		return 4;
                	}	
                }
                return 1; // Try Later
            }
        }
    }

    fclose(file);
    return 0; // Return a code indicating that the ID was not found
}

void error(char *msg) {
  perror(msg);
  exit(1);
}

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
    send(sockfd,"ok",sizeof("ok"),0);
    char reqId[10];
    if(strcmp(file_size_bytes,"REQUEST\n")==0){
    		
 	   //printf("%s\n",file_size_bytes);
 	   if (recv(sockfd, reqId, sizeof(reqId), 0) == -1){
		perror("Error receiving file size");
		fclose(file);
		return -1;
	    }
	    int targetID=atoi(reqId);
	    int fileStatus=searchRequestID(targetID,"./data/status_file.txt");
	    
	    if(fileStatus==1){ 
	    	send(sockfd,"Try Later\n",sizeof("Try Later\n"),0);
	    	close(sockfd);
	    }else if(fileStatus==2){
	    	send(sockfd,"Successful\n",sizeof("Successful\n"),0);
	    	close(sockfd);
	    }else if(fileStatus==3){
    	        char *s;
  		char s1[20];
	        s = malloc (200*sizeof(char));
                memset(s, 0, sizeof(s));
                memset(s1, 0, sizeof(s1));
  		strcpy(s, "Error/compiler_err");
 	        sprintf(s1, "%d", targetID);	
 	        strcat(s, s1);	
                strcat(s, ".txt");
                char errorInfo[500];
	    	int file = open(s,O_RDONLY);
	    	int fSize=read(file,errorInfo,sizeof(errorInfo));
	    	send(sockfd,errorInfo,sizeof(errorInfo),0);
	    	close(sockfd);
	    }else if(fileStatus==4){
	    	char *s;
  		char s1[20];
	        s = malloc (200*sizeof(char));
                memset(s, 0, sizeof(s));
                memset(s1, 0, sizeof(s1));
  		strcpy(s, "Error/runtime_err");
 	        sprintf(s1, "%d", targetID);	
 	        strcat(s, s1);	
                strcat(s, ".txt");
                char errorInfo[500];
                int file = open(s,O_RDONLY);
	    	int fSize=read(file,errorInfo,sizeof(errorInfo));
                send(sockfd,errorInfo,sizeof(errorInfo),0);
	    	close(sockfd);
	    }else{
	    	send(sockfd,"Entry for this request id not present",sizeof("Entry for this request id not present"),0);
	    	close(sockfd);
    	    }
    	    return -1;
    }	
       
    int file_size;
    //copy bytes received into the file size integer variable
    memcpy(&file_size, file_size_bytes, 4);
    
    //some local printing for debugging
    printf("File size is: %d\n", file_size);
    
    //now start receiving file data
    size_t bytes_read = 0, total_bytes_read =0;;
    while (true)
    {
    	fflush(stdout);
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
        if (total_bytes_read >= file_size-1)
            break;
    }
    fclose(file);
    return 0;
}


//Utility Function to receive a file of any size to the grading server


/*Utility Function that takes some number as an ID, and source code file name (programFile) and executable file name (execFile) as arguments, and creates a compile command such that executable file gets the name in execFile. and compiler error output if any goes into a file compiler_err<id>.txt

E.g. if the call is :

comp_comm = compile_command (2123, file2123.cpp, prog2123)

then it till return the string (comp_comm will get this string):

"gcc -o prog2123 file2123.cpp 2> Error/compiler_err2123.txt"

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
  strcat(s, " 2> Error/compiler_err");
 	sprintf(s1, "%d", id);	
 	strcat(s, s1);	
  strcat(s, ".txt");
  //printf("%s\n",s);
  return s;
}

/*Utility Function that takes some number as an ID, and executable file name (execFile) as arguments, and creates a run command such that output file gets a name such as out<id>.txt. and runtime error output if any goes into a file runtime_err<id>.txt

E.g. if the call is :

run_comm = run_command (2123, prog2123)

then it till return the string (run_comm will get this string):

"./prog2123 > out2123.txt 2> Error/runtime_err2123.txt"

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
  strcat(s, " > Output/out");
 	strcat(s, s1);	
  strcat(s, ".txt");
  strcat(s, " 2> Error/runtime_err");
 	strcat(s, s1);	
 	strcat(s, ".txt");	
  //printf("%s\n",s);
  return s;
}

/* Utility function that given a number as an argument returns a  source code file name of the format file<id>.cpp
*/

char *makeProgramFileName(int id) {

  char *s;
  char s1[20];	
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));

  sprintf(s1, "%d", id);	  
  strcpy (s, "CPP/file");
  strcat (s, s1);
  strcat (s, ".cpp");
  return s;
}  
  
/* Utility function that given a number as an argument returns an executable file name of the format prog<id>
*/
char *makeExecFileName(int id) {

  char *s;
  char s1[20];	
  
  s = malloc (200*sizeof(char));
  memset(s, 0, sizeof(s));
  memset(s1, 0, sizeof(s1));
  sprintf(s1, "%d", id);	  
  strcpy (s, "Executable/prog");
  strcat (s, s1);
  return s;
} 

void change_status_reqfile(int req_id,char* status, char* result, pthread_mutex_t request_file_lock){
    pthread_mutex_t status_file_lock;
    pthread_mutex_init(&status_file_lock,NULL);
    pthread_mutex_lock(&status_file_lock);
    FILE *file = fopen("data/request_file.txt", "r+");
    if (file == NULL) {
        perror("Failed to open the file");
        pthread_mutex_unlock(&status_file_lock);
        return;
    }
    int fileDescriptor = open("data/status_file.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fileDescriptor == -1) {
        perror("Error opening the file");
        return; // Return an error code
    }
    
    int found = 0; // To track if the request ID is found
    char requestID[256];
    char timestamp[256];
    char fstatus[256];
    char fresult[256];
    char firstLine[256];
    while (fscanf(file, "%13s %14s %11s %15s", requestID, timestamp, fstatus, fresult)==4) {
        if (atoi(requestID) == req_id) {
            found = 1;
            char data [2005];
            sprintf(data,"%-13s %-14s %-11s %-14s\n", requestID, timestamp, status, result);
            int x=write(fileDescriptor, data, strlen(data));
            break; // No need to continue reading the file
        }
    }
    if(!found)
        printf("Request ID %d not found in the file\n", req_id);
    fclose(file);
    pthread_mutex_unlock(&status_file_lock);
}

