# Server

## Overview

The server architecture has evolved from a single-threaded model to a multi-threaded model with both thread-per-request and thread pool approaches. However, a unique challenge arises from the unpredictable runtime of student-submitted code, causing potential timeouts during high load conditions. The solution lies in transforming the server into an asynchronous grading architecture.

## Asynchronous Grading Architecture

### Request Handling

1. **Queueing Mechanism:**
   - Upon receiving a grading request, the server queues the request for grading.
   - A quick response is sent to the client, acknowledging that the request is accepted and being processed.

2. **Client Response:**
   - The client's 'recv/read' call gets a rapid response, considering the request-response cycle as complete.
   - The client is no longer blocked, avoiding timeouts during unpredictable runtimes.

3. **Check Status Request:**
   - The client later sends a 'check status' request to inquire about the completion of the grading request.
   - The server responds with progress updates whenever such a request is made.

### Asynchronous Grading Task

1. **Non-Blocking Behavior:**
   - Clients no longer wait for the entire compile-run-grade process to be completed.
   - Grading tasks occur asynchronously, decoupled from the client's blocking requests.

### Request ID Implementation

1. **Identification Mechanism:**
   - To handle asynchronous status inquiries, a 'request ID' concept is introduced.
   - Clients use the assigned request ID when checking the status of a particular grading request.

## Updated Program Usage

The program usage is modified to incorporate the asynchronous grading model. The server now seamlessly manages grading tasks and provides status updates without blocking clients.

```bash
./submit <serverIP:port> <sourceCodeFileToBeGraded> <sleepTimeSeconds> <timeoutSeconds> <loopNum> <requestID>
```
#### Usage Parameters

- **serverIP:port:** IP address and port of the grading server.

- **sourceCodeFileToBeGraded:** Path to the source code file to be graded.

- **sleepTimeSeconds:** Sleep time in seconds before sending the request.

- **timeoutSeconds:** Timeout duration in seconds.

- **loopNum:** Number of times to repeat the grading request.

- **requestID:** Unique identifier for checking the status of the grading request.


# Client README

## Asynchronous Grading

The client-server interaction is modified to accommodate the asynchronous grading architecture. Clients no longer wait for the entire grading process, allowing for more flexibility during unpredictable runtimes.

## Program Usage Update

The program usage is updated to include the new asynchronous features. Clients can now initiate grading requests, receive quick responses, and check the status of the grading task later.

```bash
./submit <serverIP:port> <sourceCodeFileToBeGraded> <sleepTimeSeconds> <timeoutSeconds> <loopNum> <requestID>
```


