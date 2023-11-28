# Server Grading Application

This application is designed to accept grading requests and handle them concurrently using a fixed-size thread pool. The server is configured to listen on a specified port, and the thread pool size is determined at startup.

## Prerequisites

Before running this application, make sure you have the following installed:
- G++

## Usage

To run the server, use the following command:

`$gcc -o server gradingMTServerQ.c`
`$./server <port> <thread_pool_size>`

and the client will run as follows
`$gcc -o submit submit.c`
./submit <ip> <port> <fileLocation>

