/*
ALLAMNENI ARAVIND
(2012C6PS661P)
Undergrad @ BITS Pilani
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "rshmAPI.c"
/*
struct my_msgbuf
{
  long mtype;
  char mtext[1000];
int key;
int rshmid;
void *addr;

};
*/
struct my_msgbuf buf;

int main(void){
int msgqkey=12500;
 int msqid;

if ((msqid = msgget (msgqkey, 0644 | IPC_CREAT)) == -1)
    {
      perror ("msgget");
      exit (1);
    }
key_t key;
void *addr;
int i=0;
 while(1){

//printf("enter a key\n");
//scanf("%d",&key);
key=rand();
int rshmid=rshmget(key,1024);

addr=rshmat(rshmid);

rshmChanged(rshmid);

//rshmdt(rshmid,addr);
//i++;
}
}
