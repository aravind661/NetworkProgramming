/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf
{
  long mtype;
  char mtext[1000];
int key;
int rshmid;
void *addr;
};


int rshmget(key_t key, size_t size);
void *rshmat(int rshmid);
int rshmdt(int rshmid,void *addr);
int rshmctl(int rshmid, int cmd);
void rshmChanged(int rshmid);


struct my_msgbuf buf; 
key_t msgqkey;
int msqid;

int rshmget(key_t key, size_t size)
{
	printf("I am rshmget\n");
	buf.mtype=1;
	buf.key=key;
	buf.rshmid=shmget(key,1024,0666|IPC_CREAT);
	if (msgsnd (msqid, &(buf), sizeof(buf), 0) == -1)
		perror ("msgsnd");
	return (buf.rshmid);
}

void *rshmat(int rshmid)
{
	printf("I am rshmat\n");
	buf.mtype=2;	
	buf.rshmid=rshmid;
	buf.addr=shmat(rshmid, NULL, 0);
	if (msgsnd(msqid, &(buf), sizeof (buf), 0) == -1)
		perror ("msgsnd");
}

int rshmdt(int rshmid,void*addr){
	printf("I am rshmdt\n");
int rc;
buf.mtype=3;
buf.rshmid=rshmid;
rc = shmdt(addr);
if (msgsnd (msqid, &(buf), sizeof (buf), 0) == -1)
	perror ("msgsnd");
return rc;
}

int rshmctl(int rshmid, int cmd){
printf("I am rshmctl\n");

}


void rshmChanged(int rshmid){
	printf("I am rshmChanged %d\n",msqid);
buf.mtype=5;
buf.rshmid=rshmid;
printf("enter the message you want to send\n");
 while (gets (buf.mtext), !feof (stdin))
    {
      if (msgsnd (msqid, &(buf), sizeof (buf), 0) == -1)
	perror ("msgsnd");
    }

}




