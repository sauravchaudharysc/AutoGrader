# AutoGrader
## Introduction
In this project we will step-by-step build a scalable program autograding server (and its client). 

We can assume that the purpose of the submitted program is simply to print the first ten numbers:
1 2 3 4 5 6 7 8 9 10

If the submitted program prints this output, it has passed, else it has failed. The programming language assumed is C.

In all versions, the server will always be run as follows,

`$./server  <port>`

and the client will always be run as follows, 

`$./submit  <serverIP:port>  <sourceCodeFileTobeGraded>`

and will get back one of the following responses from the server:
1.  PASS
2.  COMPILER ERROR
3.  RUNTIME ERROR
4.  OUTPUT ERROR

In cases 2,3,4, the server should additionally send back the error details:
-  For compiler error, the entire compiler output should be sent back to the client
-  For runtime error, the error type should be sent back to the client
-  For output error, the output that the program produced, and the output of a ‘diff’ command  should be sent back to the client


## Installation Guidelines 
-  Clone the repo using `gitclone`
-  Go to the project directory
-  Run `make` to compile the code 
-  Roll the Server 
-  Roll the Client
