/*

ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani

*/
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#define MSGSIZE 1600

void int_handler1(int signo);
void int_handler2(int signo);
pid_t c1,c2;

int isTrue=0;
int isYes=0;


char istring[1000];

struct my_msgbuf
{
	long mtype;
	char mtext[1000];
int key;
int rshmid;
void *addr;
};

struct rshminfo{
int rshmid;
/*unique id across all systems. created by the
first system*/
key_t key;
/*key used to create shm segment*/
int shmid;
/*shmid returned by the local system*/
void *addr;
/*address returned by the local system*/
int ref_count;
/*no of processes attached to*/
struct sockaddr_in *remote_addrs; /* list of remote end
points*/
};

struct mesginfo{
int key;
int rshmid;
char mtext[1000];
long mtype;
};

struct my_msgbuf buf; 
	int msqid;
	key_t key;

int main(int argc,char **argv)
{
int i;
pid_t par=getpid();

struct rshminfo info[100];

int flag,status;
flag=0;
status=0;

int p[2],p1[2],p2[2],p3[2];
pipe (p);
pipe(p1);
pipe(p2);
pipe(p3);

key=12500;

c1=1;
c2=1;
c1=fork();
if(c1!=0){
c2=fork();
}

if((c1!=0)&&(c2!=0))
printf("\n i am parent, my pid is %d\npid of the c1 is %d\npid of the c2 is %d\n",par,c1,c2);

//////////////////////////// SERVER CODE //////////////////////////////////////////
if(c1==0){
int sd1, psd1;
int portNo=atoi(argv[1]);
struct   sockaddr_in name,client;
char   buf[1024],*ipaddress;
struct mesginfo message2;
int    cc,clilen;
clilen=sizeof(client);
sd1 = socket (AF_INET,SOCK_STREAM,0);
	 name.sin_family = AF_INET;
	 name.sin_addr.s_addr = htonl(INADDR_ANY);
	 name.sin_port = htons(portNo);
	 bind( sd1, (struct sockaddr*) &name, sizeof(name) );
	 listen(sd1,1);
	 psd1 = accept(sd1, (struct sockaddr *)&client, &clilen);
	 close(sd1);
	 printf("I accepted the connection and I am a server \n");

	 for(;;)
	 {
		cc=recv(psd1,buf,sizeof(buf),0) ;
		if (cc == 0) exit (0);
		buf[cc] = '\0';
		printf("message received: %s\n", buf);
		if(buf==1)
		{
			write (p[1],message2.mtype, MSGSIZE);
			cc=recv(psd1,buf,sizeof(buf),0) ;
			if (cc == 0) exit (0);
			buf[cc] = '\0';
			write (p1[1],message2.key, MSGSIZE);
			cc=recv(psd1,buf,sizeof(buf),0) ;
			if (cc == 0) exit (0);
			buf[cc] = '\0';
			write (p2[1],message2.rshmid, MSGSIZE);
			kill(par,SIGUSR2);
		}
		else if(buf==2)
		{
			write (p[1],message2.mtype, MSGSIZE);
			cc=recv(psd1,buf,sizeof(buf),0) ;
			if (cc == 0) exit (0);
			buf[cc] = '\0';
			write (p2[1],message2.rshmid, MSGSIZE);
			kill(par,SIGUSR2);

		}
		else if(buf==3)
		{

		}
		else if(buf==4)
		{

		}
		else if(buf==5)
		{
			write (p[1],message2.mtype, MSGSIZE);
			cc=recv(psd1,buf,sizeof(buf),0) ;
			if (cc == 0) exit (0);
			buf[cc] = '\0';
			write (p2[1],message2.rshmid, MSGSIZE);
			cc=recv(psd1,buf,sizeof(buf),0) ;
			if (cc == 0) exit (0);
			buf[cc] = '\0';
			write (p3[1],message2.mtext, MSGSIZE);
			kill(par,SIGUSR2);
		}




	}
}

///////////////////////////////// CLIENT CODE ////////////////////////////////////////////

if(c2==0)
{
signal (SIGUSR1, int_handler1);
int	sd,a;
int portNum=atoi(argv[2]);
	struct	sockaddr_in server;
	
	sd = socket (AF_INET,SOCK_STREAM,0);

	server.sin_family = AF_INET;
		server.sin_port = htons(portNum);
	if(inet_pton(AF_INET, argv[3], &server.sin_addr)<=0)
		{
				printf("\n inet_pton error occured\n");
				return 1;
		} 
while(1)
{
	a=connect(sd, (struct sockaddr*) &server, sizeof(server));
	wait(2);
	printf("yes! I got a connection to a server and I am a client %d\n ",a);
	if(a==0)
		break; 
}

for (;;)
{	
	if (isTrue)
	{
		read(p[0],istring,MSGSIZE);
		send(sd,istring, sizeof(istring),0);
		isTrue = 0;
		printf("client work is done\n");
	}
	
}
}

/////////////////////////////////////// PARENT CODE //////////////////////////////////

if((c1!=0)&&(c2!=0))
{
signal (SIGUSR2, int_handler2);
if ((msqid = msgget (key, 0644 | IPC_CREAT)) == -1)
		{
			perror ("msgget");
			exit (1);
		}

 //printf ("server: ready to receive messages\n");
	for (;;)
	{
		if (msgrcv (msqid, &(buf), sizeof (buf), 0, 0) == -1)
		{
			perror ("msgrcv");
			exit (1);
		}
		printf("Message by you: %ld\n%s\n", buf.mtype,buf.mtext);
		if(!isYes)
		{

		if (buf.mtype == 1)
		{
			
			for(i=0;i<100;i++)
			{
				if(info[i].key==buf.key)
				{
					printf("not found\n");
					return;
				}
				else if(info[i].key==0)	
				{
					
					break;
				}
			}
				printf("i=%d\n",i);				
				info[i].key=buf.key;
				info[i].rshmid=buf.rshmid;
				info[i].shmid=buf.rshmid;
				info[i].ref_count=1;

				// if type 1 rshmget
				write (p[1],buf.mtype, MSGSIZE);
				kill(c2,SIGUSR1);
				write (p[1],buf.key, MSGSIZE);
				kill(c2,SIGUSR1);
				write (p[1],buf.rshmid, MSGSIZE);
				kill(c2,SIGUSR1);

		}else if(buf.mtype == 2)
		{
			// if type 2

				for(i=0;i<100;i++)
			{
				if(info[i].rshmid==buf.rshmid)
				{
					info[i].addr=buf.addr;
					write (p[1],buf.mtype, MSGSIZE);
					kill(c2,SIGUSR1);
					write (p[1],buf.rshmid, MSGSIZE);
					kill(c2,SIGUSR1);
					break;


				}
				else if(info[i].rshmid==0)	
				{
					break;
				}
			}
			
				

		}else if (buf.mtype == 3)
		{
			//
			write (p[1],buf.mtype, MSGSIZE);
				kill(c2,SIGUSR1);

		}else if (buf.mtype == 4)
		{
			//
			write (p[1],buf.mtype, MSGSIZE);
				kill(c2,SIGUSR1);
		}else if (buf.mtype == 5)
		{
			//
			for(i=0;i<100;i++)
			{
				if(info[i].rshmid==buf.rshmid)
				{
					strcpy(info[i].addr,buf.mtext);
					//return;
				}
				else if(info[i].key==0)	
				{
					break;
				}
			}
				write (p[1],buf.mtype, MSGSIZE);
				kill(c2,SIGUSR1);
				write (p[1],buf.rshmid, MSGSIZE);
				kill(c2,SIGUSR1);
				write (p[1],buf.mtext, MSGSIZE);
				kill(c2,SIGUSR1);
		}
		
		}
		else if(isYes)
		{
			read (p[0],buf.mtype, MSGSIZE);
			if(buf.mtype==1)
			{
				read (p1[0],buf.rshmid, MSGSIZE);
				read (p2[0],buf.key, MSGSIZE);
			for(i=0;i<100;i++)
			{
				if(info[i].key==buf.key)
				{
					return;
				}
				else if(info[i].key==0)	
				{
					break;
				}
			}
				info[i].key=buf.key;
				info[i].rshmid=buf.rshmid;
				info[i].ref_count=0;
				info[i].shmid=shmget(buf.key,1024,0666|IPC_CREAT);
				isYes=0;
				continue;
			}
			else if(buf.mtype==2)
			{
				read (p1[0],buf.rshmid, MSGSIZE);
				for(i=0;i<100;i++)
			{
				if(info[i].rshmid==buf.rshmid)
				{
					info[i].addr=shmat(info[i].shmid,NULL,0);
					info[i].ref_count++;
				}
				else if(info[i].rshmid==0)	
				{
					break;
				}
			}
			}
			else if(buf.mtype==3)
			{

			}
			else if(buf.mtype==4)
			{

			}
			else if(buf.mtype==5)
			{
				read (p1[0],buf.rshmid, MSGSIZE);
				for(i=0;i<100;i++)
				{
					if(info[i].rshmid==buf.rshmid)
					{
					strcpy(info[i].addr,buf.mtext);
					printf("message added to shared memory %s\t%d",buf.mtext,info[i].shmid);
					}
					else if(info[i].rshmid==0)	
					{
					break;
					}
				}	
			}
		}
	}
	
}
while ((flag = wait(&status)) > 0)
{
}


}


void int_handler1(int signo){
	isTrue=1;
}

void int_handler2(int signo){
	isYes=1;
}
