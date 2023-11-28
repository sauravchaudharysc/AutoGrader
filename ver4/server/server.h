#ifndef SERVER_H
#define SERVER_H
#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE_BYTES 400
#define MAX_LINE_LENGTH 100
#define QUEUE_SIZE 20

struct LogEntry {
    int ID;
    int TimeStamp;
    char Status[20];
    char Result[20];
};
int generate_request_id();
int searchRequestID(int targetID, const char* filename);
void error(char *msg);
int recv_file(int sockfd, char* file_path);
char* compile_command(int id, char* programFile, char* execFile);
char* run_command(int id, char* execFileName);
char *makeProgramFileName(int id);
char *makeExecFileName(int id);
void change_status_reqfile(int req_id,char* status, char* result, pthread_mutex_t request_file_lock);

#endif




