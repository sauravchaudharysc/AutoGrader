# AutoGrader
## Version 1

### SETUP
The server will run as follows,

`$./server  <port>`

and the client will run as follows, 

`./submit <serverIP:port> <sourceCodeFileTobeGraded> <sleepTimeSeconds> <timeout-seconds> <loopNum>`


### `Server`
In gradingserver.c we receive the portno from the command line.

We first create the socket and then bind it with the respective port no. Then we listen for incoming connection request.

On receiving the connection request we accept the connection and create a new sockfd.

Now we are adding multithreading capability to the server. I.e. now you have a listener thread that
accepts grading requests and creates a worker thread to process each request. A thread
should be created for each request and should exit after serving one autograding
request. The thread should directly write the response back to the client and then exit.


### `Client`
In gradingclient.c we receive the serverIp , portno and name of file to be graded from the command line.

We first create the socket, fill in the server address and then connect to server. 

After successfull connection, we read the file either wholly and write it at sockfd(Socket which connects client and server) .


