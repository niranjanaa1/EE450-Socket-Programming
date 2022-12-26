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

#define PORTCLIENT "34484"     // the port for TCP with client

#define UDPPORT "33484" // the port use UDP

#define SERVERPORTA "30484"	   // the port users will be connecting to 
							   // Backend-Server (A)
#define SERVERPORTB "31484"	   // the port users will be connecting to 
							   // Backend-Server (B)
#define SERVERPORTC "32484"	   // the port users will be connecting to 
							   // Backend-Server (C)
#define IPADDRESS "127.0.0.1"  // local IP address

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAPNOTFOUND "Map not found"

#define MAXBUFLEN 4000

// from beej
void sigchld_handler(int s){
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    
    while(waitpid(-1, NULL, WNOHANG) > 0);
    
    errno = saved_errno;
}

// funciton to get socket address from a sockaddr struct, from beej
void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET6) {
        // IPv6
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
    // IPv4
    return &(((struct sockaddr_in*)sa)->sin_addr);
}


// setup TCP with client at port
int setupTCP(char* port) {
    int rv;
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes = 1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(IPADDRESS, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo);

    if(p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    sa.sa_handler = sigchld_handler; // reap all dead processes sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1); 
    }

    return sockfd;
}

int setupUDP2(char* port)
{
	int sockfd;
	int rv;
	struct addrinfo hints, *servinfo, *p;
	socklen_t addr_len;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(IPADDRESS, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	freeaddrinfo(servinfo); // done with servinfo
	return sockfd;
}

// use the same UDP socket to query based on specified port information
void udpQuery2(int sockfd, char *query, char *port, char *data) {
    //int server_sock;
    int numbytes;
    int rv;
    struct addrinfo hints, *servinfo, *p;
	socklen_t addr_len;
	memset(&hints, 0, sizeof hints);
    char recv_data[MAXBUFLEN]; // data received from backend server
    
    if ((rv = getaddrinfo(IPADDRESS, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

    for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}
        break;
    }

    if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return;
	}

	if ((numbytes = sendto(sockfd, query, strlen(query), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

    int recv_bytes;

    recv_bytes = recvfrom(sockfd, recv_data, sizeof recv_data, 0, NULL, NULL);
    if(recv_bytes == -1) {
        perror("recvfrom");
        exit(1);
    }
    recv_data[recv_bytes] = '\0';
    strcpy(data, recv_data);
}


int main(void) {
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	int udp_sock;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size; // used for accept()
	char s[INET6_ADDRSTRLEN];
	int numbytes; //using in receive or send

    char message_buf[100];  // temp saving place for receiving from client though TCP
    char result_buf[100]; // temp saving place for sending results to client though TCP
    char udp_send[50];
    char udp_received[MAXBUFLEN],udp_received1[MAXBUFLEN];  // buffer to receive map data from backend server
    char queryC[MAXBUFLEN];      // buffer to merge args from client and data from server A or B
    char *splited_args[4];
    char *parsed_results[5];    // {shortest_dist, trans_delay, prop_delay, total_delay, ans_path}

    int has_map_id = 0;
    int vtx_valid = 0;

	sockfd = setupTCP(PORTCLIENT);
    udp_sock = setupUDP2(UDPPORT);

	printf("The main server is up and running.\n");
    
    printf("server: waiting for connections...\n");

        has_map_id = 0;
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if(new_fd == -1) {
            perror("accept");
            // continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

       

            // receive arguments as single string
            if((numbytes = recv(new_fd, message_buf, sizeof message_buf, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            message_buf[numbytes] = '\0';
            fflush(stdout);
            printf("from tcp client %s",message_buf);
            char newString[10][10];
            char received[100];
            strcpy(received,message_buf);
            int i,j=0; int ctr=0;
            for(i=0;i<=(strlen(received));i++)
            {
                // if space or NULL found, assign NULL into newString[ctr]
                if(received[i]==' '||received[i]=='\0')
                {
                    newString[ctr][j]='\0';
                    ctr++;  //for next word
                    j=0;    //for next word, init index to 0
                }
                else
                {
                    newString[ctr][j]=received[i];
                    j++;
                }
            }
            printf("The main server received the authentication for %s using TCP over port %d",newString[0],PORTCLIENT);
            // printf("\n newstring: %s %s\n",newString[0],newString[1]);
            char encrypted[50];
            int k=0;
            for(i=0;i<strlen(newString[1]);i++)
            { k = newString[1][i];
            if( (k>=65 && k<=90) || (k>=97 && k<=122))
            {
                if (k == 90 || k==87 || k==88 || k == 89 || k == 119 || k ==120 || k==121 || k==122)
                {k = (k - 26) ; }
                k+=4;
            }
            else if( k>=48 && k<=57)
            {
                if (k == 54 || k == 55 || k == 56 || k == 57 )
                    {k = k -10;}
                k+=4;
            }
            encrypted[i] = k;
            //   printf("%c",k);

            } 
            char auth[100]; 
            // printf("encrypted: %s",encrypted);
            sprintf(auth, "%s %s", newString[0],encrypted);
            printf("combine string: %s", message_buf);
            char ass[10];
            //gets(ass);
            
            udpQuery2(udp_sock, auth, SERVERPORTA, udp_received);
            printf("The main server sent an authentication request to serverC.");
            printf("The main server received the result of the authentication request from ServerC using UDP over port %d", SERVERPORTA);

            if (send(new_fd, udp_received, strlen(udp_received), 0) == -1) {
                            perror("send");
                        }
            printf("The main server sent the authentication result to the client.");
            if((numbytes = recv(new_fd, message_buf, sizeof message_buf, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            message_buf[numbytes] = '\0';
            fflush(stdout);
            printf("The main server received from %s to query course %s using TCP over port %d",newString[0],message_buf, PORTCLIENT);

            //process and split to c and d 
            udpQuery2(udp_sock, message_buf, SERVERPORTB, udp_received);
            printf("The main server sent a request to server<EE or CS>.");

            udpQuery2(udp_sock, message_buf, SERVERPORTC, udp_received1);

            sprintf(udp_send, "%s %s", udp_received, udp_received1);
            printf("The main server received the response from server<EE or CS> using UDP");
            if (send(new_fd, udp_send, strlen(udp_send), 0) == -1) {
                            perror("send");
                        }
            printf("The main server sent the query information to the client.");
            gets(ass);
            exit(1);


    

    return 0;
}

