/*
ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani

Problem Statement:
Write a program webserver.c for concurrent web server supporting HTTP GET requests.
It supports concurrency through a event-driven model, using epoll() edgetriggered notifications.

1) server maintains a message queue to queue the events.
2) main thread waits on epoll_wait() to get IO notifications from kernel. For each IO notification, it adds an event to the message queue.
3) "process" thread waits on message queue msgrcv() call. It gets a message and responds according to its state.
    While processing the message it may add new events to the queue.
4) each client request goes through states: READING_REQUEST, HEADER_PARSING, READING_DISKFILE, WRITING_HEADER, WRITING_BODY, DONE.
  Next state is reached only after the previous one is complete.
5) All IO (read and write) operations are non-blocking. This necessitates buffer management.
6) data pertaining to client's request is stored in a central data structure such as hash table.
  This is to avoid unnecessary copying of data which may happen if we keep client data in the message queue itself.
7) When a request reaches DONE state, connection remains open for another request.
  Client connection is closed only when EOF is received from client. That may happen during any state.
8) web server testing tools such apache ab or httpperf can be used as clients.

*/

#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>



#define READING_REQUEST 1
#define HEADER_PARSING 2
#define READING_DISKFILE 3
#define WRITING_HEADER 4
#define WRITING_BODY 5
#define DONE 6


extern int errno;
static void *threadHandler(void );

#define LISTENQ 5
#define MAX_BUF 10		/* Maximum bytes fetched by a single read() */
#define MAX_EVENTS 5		/* Maximum number of events to be returned from
				   a single epoll_wait() call */
void
errExit (char *s)
{
  perror (s);
  exit (1);
}

struct my_msgbuf
{
  long mtype;
  char mtext[1024];
  int mfd;
  char message[10240];
  char body[10240];
};

struct my_msgbuf buff;
  int msqid;
  key_t key=12300;
 
int main (int argc, char *argv[])
{

pthread_t thread;
int retVal;
retVal=pthread_create(&thread,NULL,&threadHandler,NULL);
if ((msqid = msgget (key, IPC_CREAT | 0644)) == -1)
    {
      perror ("msgget");
      exit (1);
    }

printf("msgq created\n");
printf("msqid is:%d",msqid);
  
  int epfd, ready, fd, s, j, num0penFds;
  struct epoll_event ev;
  struct epoll_event evlist[MAX_EVENTS];
  char buf[MAX_BUF];
  int listenfd, clilen;


  struct sockaddr_in cliaddr, servaddr;

  listenfd = socket (AF_INET, SOCK_STREAM, 0);

  bzero (&servaddr, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (atoi (argv[1]));

  if (bind (listenfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
    perror ("bind");

  listen (listenfd, LISTENQ);



  if (argc < 2 || strcmp (argv[1], "--help") == 0)
    printf ("Uage: %s <port>\n", argv[0]);

  epfd = epoll_create (20);
  if (epfd == -1)
    errExit ("epoll_create");


  ev.events = EPOLLIN;		/* Only interested in input events */
  ev.data.fd = listenfd;
  if (epoll_ctl (epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    errExit ("epoll_ctl");
  for (;;)
    {ready=-1;
      ready = epoll_wait (epfd, evlist, MAX_EVENTS, -1);
      if (ready == -1)
	{
	  if (errno == EINTR)
	    continue;		/* Restart if interrupted by signal */
	  else
	    errExit ("epoll_wait");
	}
      printf("nready=%d\n", ready);

      for (j = 0; j < ready; j++)
	{
	  if (evlist[j].events & EPOLLIN)
	    {
	      if (evlist[j].data.fd == listenfd)
		{
		  clilen = sizeof (cliaddr);
		  char ip[128];
		  memset (ip, '\0', 128);
		  int connfd =
		    accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

		  if (cliaddr.sin_family == AF_INET)
		    {
		      if (inet_ntop (AF_INET, &(cliaddr.sin_addr), ip, 128) ==
			  NULL)
			perror ("Error in inet_ntop\n");
		    }

		  if (cliaddr.sin_family == AF_INET6)
		    {
		      inet_ntop (AF_INET6, &(cliaddr.sin_addr), ip, 128);
		    }

		  printf ("new client: %s, port %d\n", ip,
			  ntohs (cliaddr.sin_port));


		  ev.events = EPOLLIN;	/* Only interested in input events */
		  ev.data.fd = connfd;
		  if (epoll_ctl (epfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
		    errExit ("epoll_ctl");
		}
	      else
		{

			buff.mtype=READING_REQUEST;
			buff.mfd=evlist[j].data.fd;
			
//printf("buff.mtype=%d\tbuff.mfd=%d\n",buff.mtype,buff.mfd);
//printf("msqid is:%d",msqid);
if (msgsnd (msqid, &(buff.mtype), sizeof (buff.mtype), 0) == -1)
{
		
	//perror ("msgsnd");
}			


/*
		  int s = read (evlist[j].data.fd, buf, MAX_BUF);
		  buf[s] = '\0';
		  if (s == -1)
		    errExit ("read");
		  if (s == 0)
		    {
		      close (evlist[j].data.fd);
		    }
		  if (s > 0)
		    write (evlist[j].data.fd, buf, strlen (buf));

*/		}
	    }
	}
    }
}

void *threadHandler(void){
 if ((msqid = msgget (key, 0644 | IPC_CREAT)) == -1)
    {
      perror ("msgget");
      exit (1);
    }
pthread_t tid;
tid=pthread_self();
printf("i am thread named thread and id:%d\n",tid);
struct my_msgbuf buff1,buff2;
char buffer[1024];
char method[sizeof (buffer)];
char url[sizeof (buffer)];
char protocol[sizeof (buffer)];

while(1){

if (msgrcv (msqid, &(buff1.mtype), sizeof (buff1.mtype), 0, 0) == -1)
	{
	  perror ("msgrcv");
	  exit (1);
	}

if(buff1.mtype==READING_REQUEST){
printf("reading request");
int cc;
cc=recv(buff1.mfd,buffer,sizeof(buffer),0) ;
buff2.mtype=HEADER_PARSING;
buff2.mfd=buff1.mfd;
strcpy(buff2.mtext,buffer);
if (msgsnd (msqid, &(buff2.mtype), sizeof (buff2.mtype), 0) == -1)
			perror ("msgsnd");


}
else if(buff1.mtype==HEADER_PARSING){
printf("header parsing");
sscanf (buff2.mtext, "%s %s %s", method, url, protocol);
buff2.mtype=READING_DISKFILE;
buff2.mfd=buff1.mfd;
if (msgsnd (msqid, &(buff2.mtype), sizeof (buff2.mtype), 0) == -1)
			perror ("msgsnd");

}

else if(buff1.mtype==READING_DISKFILE){
printf("reading diskfile");
FILE*fp=fopen(buff1.mtext,"r");
char c;
int i=0;
char msg[10240];
while((c = fgetc(fp)) != EOF) {
		msg[i]=c;
		i++;
	}
buff2.mtype=WRITING_HEADER;
buff2.mfd=buff1.mfd;
strcpy(buff2.body,msg);
if (msgsnd (msqid, &(buff2.mtype), sizeof (buff2.mtype), 0) == -1)
			perror ("msgsnd");

}
else if(buff1.mtype==WRITING_HEADER){
printf("writing header");
strcat(buff2.message,"HTTP/1.1 200 OK\r\n");
buff2.mtype=WRITING_BODY;
buff2.mfd=buff1.mfd;
strcpy(buff2.body,buff1.body);
if (msgsnd (msqid, &(buff2.mtype), sizeof (buff2.mtype), 0) == -1)
			perror ("msgsnd");

}
else if(buff1.mtype==WRITING_BODY){
printf("writing body");
strcat(buff2.message,"\r\n\r\n");
strcat(buff2.message,buff1.body);
buff2.mtype=DONE;
buff2.mfd=buff1.mfd;
if (msgsnd (msqid, &(buff2.mtype), sizeof (buff2.mtype), 0) == -1)
			perror ("msgsnd");


}

else if(buff1.mtype==DONE){

send(buff1.mfd,buff1.message,sizeof(buff1.message),1);


}




}


}


