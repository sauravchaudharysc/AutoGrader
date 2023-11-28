# Server Grading Application

This application is designed to handle concurrent grading requests using a fixed-size thread pool. The server is configured to listen on a specified port, and the thread pool size is determined at startup.

## Prerequisites

Before running this application, make sure you have the following installed:

- G++

## Usage

To run the server, follow these steps:

1. Compile the server source code:

    ```bash
    $ gcc -o server gradingMTServerQ.c
    ```

2. Run the server with the specified port and thread pool size:

    ```bash
    $ ./server <port> <thread_pool_size>
    ```

To submit grading requests, you can use the client application. Here's how to run the client:

1. Compile the client source code:

    ```bash
    $ gcc -o submit submit.c
    ```

2. Run the client with the server's IP address, port, and the location of the file to be graded:

    ```bash
    $ ./submit <ip> <port> <fileLocation>
    ```

Make sure to replace `<port>`, `<thread_pool_size>`, `<ip>`, and `<fileLocation>` with the appropriate values.


