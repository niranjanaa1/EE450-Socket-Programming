
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

#define IPADDRESS "127.0.0.1"  // local IP address
#define MYPORT "31484"	// the port used for UDP connection with AWS
#define MAXBUFLEN 4000


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    char buf[MAXBUFLEN];
    char data[MAXBUFLEN];
    char *args[3];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    // get information of the backend server itself with MYPORT
    if((rv = getaddrinfo(IPADDRESS, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if(p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    addr_len = sizeof their_addr;
    
    printf("The Server CS is up and running using UDP on port %s number>.\n", MYPORT);
    fflush(stdout);

    
        if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("The Server CS received a request from the Main Server about the %s\n",buf);
        FILE* ptr;
        char myText[100];
        strcpy(myText, buf);
        char result[100];

        // Read from the text file
        ptr = fopen("cvscs.txt", "r");
        if(ptr == NULL){
        printf("File cant be opened");
        exit(1);
        }

        // Use a while loop together with the getline() function to read the file line by line
            char *line = NULL;
            size_t len = 0;
        
            while(getline(&line, &len, ptr) != -1) {
                //  printf("%s \n", line);
                char* token = strtok(line, ",");
                //  printf("%s\n",token);
                if(strcmp(token,myText) == 0){
                while( token != NULL ) {
                    //printing each token
                    // printf( " %s\n", token );
                    strcat(result,token);
                    token = strtok(NULL, ",");
                    
                    }
                }
                //  printf("line length: %zd\n", strlen(line));
            }
            printf("The course information has been found: The information is %s", result);
            if(strcmp(result,"") == 0){
                printf("Didn’t find the course: %s", buf);
            }
        numbytes = sendto(sockfd, result, strlen(result), 0,
            (struct sockaddr *)&their_addr, addr_len);
        printf("The Server CS finished sending the response to the Main Server");
        if(numbytes == -1) {
            perror("listener: sendto");
            exit(1);
        }
    return 0;

}
