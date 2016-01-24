/*
ARAVIND ALLAMNENI
Undergrad @ BITS Pilani
09772211572

Problem Statement: 

Write a concurrent TCP server using select() that does the following:
1) it takes port no on command-line and waits for incoming connections on that port. 
2) it uses select() to handle concurrency.
3) telnet application can be used a client.
4) Whenever it receives a message from a client it sends it to rest of the clients connected to it.
5) At every 20 seconds server sends "hello" message to all the clients connected to it. 
6) If it doesn't get reply within 10 seconds from a client, it will close the connection to that client.    

Files Expected: tcpserver.c and makefile.

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>

#define SIZE 50

#define WAITTIME 5

int client[FD_SETSIZE];

void handler20sec(int signumber);

int main(int argc, char *argv[])
{
	signal(SIGALRM, handler20sec);

	// childpid
	pid_t childpid;

	// listen descriptor and connected descroptor
	int lfd, connfd, sockfd;

	// max value of descriptor and index of the clientdescriptors array
	int maxfd, maxindex;

	// loop variables
	int i,j,k;
	
	// descriptor set to be passed into the select call
	fd_set tempset, savedset;

	// array to store all client descriptors
	int numready, numtodelete;

	// servr address, client address
	struct sockaddr_in serv_addr, client_addr;
	
	// message to be sent to clients
	char msg[SIZE];

	if (argc != 2)
	{
		printf("Input error: Port numebr not provided>\n");
	}

	// create a socket
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0)
	{
		printf("socket() error\n");
	}

	// zero out the server address space
	bzero((char *)&serv_addr, sizeof(serv_addr));

	// fill the server address with appropriate values
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		// print bind error
		printf("bind() error\n");
		exit(-1);
	}

	// listen on the created socket
	listen(lfd, 10);

	// initially maxfd = lfd
	maxfd = lfd;

	// when the client descriptor array is empty index = -1
	maxindex = -1;
	
	for (i=0; i<FD_SETSIZE; i++)
	{
		//initialise all values to -1
		client[i] = -1;
	}
	
	// zero the fdset
	FD_ZERO(&savedset);
	
	// add lfd to monitor
	FD_SET(lfd, &savedset);
	//alarm(WAITTIME);
	//j = 0;
	// Infinitely loop the server
	struct timeval timeout, timeout1;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
	timeout1.tv_sec = 10;
	timeout1.tv_usec = 0;
 	while(1)
	{
		// use another set inside this scope to avoid overeriting
		tempset = savedset;

		printf("\nServer waiting... .  .   .%d\n", (int )timeout.tv_sec);

		

		int numready1 = select(maxfd+1, &tempset, NULL, NULL, &timeout1);
		
		// Incase of select returns in   10 seconds
		if ((timeout1.tv_sec == 0))
		{
			timeout1.tv_sec = 10;
			printf("Afer 10 sec terminate \n");
			for (k = 0; k < FD_SETSIZE; k++)
			{
				int fdr;
				if ((fdr = client[k]) < 0)
				{
					continue;
				}
				if(FD_ISSET(fdr, &tempset))
				{
					continue;
				}
				else
				{
					// close fron server side
					close(fdr);
					// remove from client list
					FD_CLR(fdr, &savedset);
					client[k] = -1;
				}
			}
			
		}



		// select retirns the descriprot ready with data
		numready = select(maxfd+1, &tempset, NULL, NULL, &timeout);
		//numready = select(maxfd+1, NULL, &tempset, NULL, &timeout1);
		//printf("%d\n", (int )timeout.tv_sec);

		

		// Incase of timeout send hello every 20 sec to all clients.
		if (timeout.tv_sec == 0)
		{

			printf("TO send hello every 20 sec\n");
			for (i = 0; i < FD_SETSIZE; i++)
			{			
				if (client[i] > 0)
				{
					write(client[i], "hello\n", sizeof("hello\n"));
				}
			}
			timeout.tv_sec = 20;
		}
		
		
		

		// New connection arrives
		if(FD_ISSET(lfd, &tempset))
		{
			// Handle the New connection
			printf("New client connection...");
			int size = sizeof(client_addr);
			connfd = accept(lfd, (struct sockaddr *)&client_addr, &size);
			if (connfd > 0)
			{
				printf("Accepted\n");
			}

			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}

			if (i == FD_SETSIZE)
			{
				printf("Error:Too many clients\n");
				exit(-1);
			}

			/* NO FORKING REQUIRED
			childpid = fork();
			if (childpid == 0)
			{
				// Child Process
				close(lfd);
			}
			if (childpid > 0)
			{
				// Parent Process
			}
			*/
			
			// add lfd to the savedset
			FD_SET(connfd, &savedset);
			
			// update maxfd
			if(connfd > maxfd)
			{
				maxfd = connfd;
			}

			// update maximum index in the client array
			if(i > maxindex)
			{
				maxindex = i;
			}
		

			// nomore readable descriptors
			if (--numready <= 0)
			{
				continue;	
			}
		}

		// New data arrives
		// loop through the clients array to find which one sent data data
		for(k=0; k<=maxindex; k++)
		{	
			
			if ((sockfd = client[k]) < 0)
			{
				continue;
			}
			
			if(FD_ISSET(sockfd, &tempset))
			{
				int num_bytes;
				if( (num_bytes = read(client[k], msg, SIZE)) > 0)
				{
					msg[num_bytes] = '\0';
					//msg[num_bytes+1] = '\n';
					printf("Received: %s \n", msg);
					//write(sockfd, msg, SIZE);
					//char *msg1 = msg;
					for (i = 0; i < FD_SETSIZE; i++)
					{
						if ((client[i] > 0) && client[i] != client[k])
						{
							write(client[i], msg, SIZE+1);
							//write(client[i], "\0", 1);
						}
					}
				}

				// Connection closed by the cient
				if(num_bytes == 0)
				{
					// close fron server side
					close(sockfd);
					// remove from client list
					FD_CLR(sockfd, &savedset);
					client[k] = -1;
				}

				// break when all descriptors returned by select are handled
				if(--numready <= 0)
					break;
			}
			
		}
	} 
	// end main listening loop
	close(lfd);
	return 0;
}

void handler20sec(int signumebr)
{	
	alarm(WAITTIME);
	int i;
	//char[10] = "hell0";
	for (i = 0; i < FD_SETSIZE; i++)
	{
		if (client[i] > 0)
		{
			write(client[i], "hello\n", sizeof("hello\n"));
		}
	}
}
