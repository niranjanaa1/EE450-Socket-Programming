/**
**
** client.c
** Author: Ruihao Wang
** USC-ID#: 9867439484
** Code is modified and expanded base on stream client source in beej
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <ctype.h>

#define PORT "34484"    // the TCP port of AWS which client connects to
#define IPADDRESS "localhost" // source and destination address
#define MAXBUFLEN 4000   // the maximum number of bytes


// funciton to get socket address from a sockaddr struct
void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET6) {
        // IPv6
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
    // IPv4
    return &(((struct sockaddr_in*)sa)->sin_addr);
}



int main() {

    int sockfd, numbytes;  
    char buf[MAXBUFLEN];
    char *results[5];
    char args_str[100];
    char check[10];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(IPADDRESS, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("The client is up and running.\n");

    freeaddrinfo(servinfo); 
    char uname[20],pass[20];
    printf("\nPlease enter your username: ");
    gets(uname);
    printf("\nPlease enter your password: ");
    gets(pass);


    sprintf(args_str, "%s %s", uname, pass);

    if ((numbytes = send(sockfd, args_str, strlen(args_str), 0)) == -1) {
        perror("send");
        exit(1);
    }
    fflush(stdout);
    printf("%s sent an authentication request to the main server.",uname);



    if((numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("%s",buf);

    if(strcmp(buf,"invaliduser")==0)
    {
        printf("\n %s received the result of authentication using TCP over port %d. Authentication failed: Username Does not exist", uname,PORT);
        exit(1);
    }
    if(strcmp(buf,"invalidpassword")==0)
    {
        printf("\n %sreceived the result of authentication using TCP over port %d. Authentication failed: Password does not match", uname,PORT);
        exit(1);
    }
    printf("%s received the result of authentication using TCP over port %d. Authentication is successful",uname,PORT);
        //get input
    printf("\nPlease enter the course code to query: \n");
    char courses[100];
    gets(courses);

    if ((numbytes = send(sockfd, courses, strlen(courses), 0)) == -1) {
        perror("send");
        exit(1);
    }
    fflush(stdout);

    printf("%s sent a request to the main server",uname);

    if((numbytes = recv(sockfd, buf, MAXBUFLEN-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("The client received the response from the Main server using TCP over port %d\n", PORT);
    printf("%s",buf);
    

    close(sockfd);

    return 0;
}
