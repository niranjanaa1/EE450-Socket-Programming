# EE450-Socket-Programming
EE 450 PROJECT PROGRAMMING - Niranjanaa Mohanbabu - 2485318921

This is the repository collects the code of Fall 2022 EE450 Socket Programming Project. The project is C/C++ based and aims to simulate the client-server communication with TCP and a main server to its backend servers using UDP.
I have already finished requires phases in description:

Phase 1A : Client sends an authentication request to the main server .

Phase 1B: Main server forwards the authentication request to the credentials server over UDP.

Phase 2A: serverC sends the result of the authentication request to serverM over a UDP connection.

Phase 2B: serverM sends the result of the authentication request to the client over a TCP connection.
Phase 3a: Finish the data transmission from client to serverM by sending a query for course Phase 3B: The main server will send the query information to the backend department server via UDP connection.
Phase 4a: The responsible Department server should have the query information ready. The query information is first sent back to the Main server using UDP.
Phase 4B: The main server will forward the result to the Client using TCP and print out an on-screen message.

CODE FILES AND THEIR FUNCTIONS:

* client.c

Code for client to communicate with serverM by TCP. Transmit the course code and file size read from command line. Request result from serverM.

* serverM.c

Code for serverM which have the following functions:

1. Receives the information from a client with TCP.

2.Encrypts the Username and Password and send it to server C

Send QUERY to backend sever EE/CS for course information with UDP.

If available, send the necessary information to backend server C with UDP.

Send the client the result figured out by backend server C.

* serverC.c

Gets the encrypted username and password from Main Server to send back the following :

0 if the authentication request has failed,

1 if password has failed or empty

2 if the request has passed * serverEE.c

Gets the query from serverM and returns the information requested by client from the EE text file.

* serverCS.cpp :

Same as EE serverCS but it gets information from CS text file


Format of message exchange.

Console output sample of client

The client is up and running.

The client has sent query to serverM using TCP:

2. Console output sample of serverM

The main server is up and running

The main server received the authentication for <username> using TCP over port <port number>.

The serverM sent an authentication request to serverC.

The serverM has received the result of the authentication request from ServerC using UDP over port 21921

The serverM sent the authentication result to the client.

The serverM received from <uname> to query course <code> about <category> using TCP over port 24921.

The main server sent a request to server EE or CS.

The main server received the response from server using UDP over port 22921 or 23921

The main server sent the query information to the client:

3. Console output sample of serverC

The serverC is up and running using UDP on port 21921

The serverC received an authentication request from the Main Server

The serverC finished sending the response to the Main Server

4. Console output sample of serverCS

The serverCS is up and running using UDP on port 22921

The serverCS received a request from the Main Server about the <category> of <course code> The course information has been found: The <category> of <course code> is <information> //If course not found : Dint find <course code>

The Server CS finished sending the response to the Main Server.

5. Console output sample from serverEE

The serverEE is up and running using UDP on port 22921

The serverEE received a request from the Main Server about the <category> of <course code> The course information has been found: The <category> of <course code> is <information> //If course not found : Dint find <course code>

The ServerEE finished sending the response to the Main Server.

The Server C finished sending the output to serverM

REFERENCES:

The implementation of TCP and UDP refers to Beej’s Guide to Network Programming. 
The variable names are quite similar. The implementation of authentication and file input outputs are based on code from GeeksforGeeks with some modifications.
https://www.geeksforgeeks.org/socket-programming-cc/
https://github.com/kusdavletov/socket-programming-simple-server-and-client
https://github.com/MAGI003769/EE450-Socket-Project
https://www.youtube.com/watch?v=LtXEMwSG5-8
https://www.youtube.com/watch?v=LtXEMwSG5-8&list=RDCMUCAQjmY2DJqwU3Eqz0oN83aw&start_radio=1&rv=LtXEMwSG5-8
https://youtu.be/gntyAFoZp-E
https://www.youtube.com/watch?v=sXW_sNGvqcU

