# AutoGrader
## Version 1

### `Server`
In gradingserver.c we receive the portno from the command line.

We first create the socket and then bind it with the respective port no. Then we listen for incoming connection request.

On receiving the connection request we accept the connection and create a new sockfd.

We receive the message of client via the newsockfd and start performing operation on it.

And after certain operations we write the result on mutual socket fd between both client and server,

### `Client`
In gradingclient.c we receive the serverIp , portno and name of file to be graded from the command line.

We first create the socket, fill in the server address and then connect to server. 

After successfull connection, we read the file either wholly and write it at sockfd(Socket which connects client and server) or we can perform the file read line by line and on side writing the information on sockfd line by line
