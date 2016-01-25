/*
ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani
*/
	#include <stdio.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <sys/shm.h>
	#include <signal.h>

	void int_handler1(int signo);



	typedef struct{

	pid_t pid;
	int noOfMsgs;
	}msgCount;
	msgCount msgsCount[10];

	// shared memory constants
	#define SHMKEYPATH "./shmserver.c"
	#define SHMSEGSIZE 1024
	#define SHMKEYID 1

	// semaphore constants
	#define SEMKEYPATH "./shmserver.c"
	#define SEMKEY 2012661
	#define SEMKEYID 1
	#define NUMOFSEMS 3

	int main(int argc, char const *argv[])
	{
	signal(SIGINT,int_handler1);
	int i=0;
		// pid 
		pid_t serverpid = getpid();

		// type of message written to shared memory
		




	typedef struct
		{
			int type;
			int pid;	// clients pid
			int slno;	// incremented for every message
			int a;	 // any number
			int b;	// any nnumber
			int total;	//total of a and b processed by server
		}message;

		// shared memory variables
		int shmid;
		key_t shmkey;
		message *msg;

		// semaphore variables
		int semid;
		key_t semkey;

		//creating a key for shared memory
		//shmkey = ftok(SHMKEYPATH, SHMKEYID);

		// creating a shared memory segment using shmget()
		shmid = shmget(2012700, SHMSEGSIZE, 0666 | IPC_CREAT);
		if (shmid == -1)
		{
			perror("Shared memory not created.\n");
		}

		// Attaching shared memory
		/*msg = (message *)shmat(shmid, NULL, 0);

		msg->type = 1;
		msg->pid = serverpid;
		msg->a = 9;
		msg->b = 8;
		msg->total = 1;
		
		printf("Initial Msg: a=%d, b=%d, tot=%d.\n", msg->a, msg->b, msg->total);
		*/
		// creating a key for semaphore
		//semkey = ftok(SEMKEYPATH, SEMKEYID);

		// creating s semaphore
		semid = semget(SEMKEY, NUMOFSEMS, 0666 | IPC_CREAT );
		if (semid == -1)
		{
			perror("Semaphore not created.\n");
		}

		int p;
		p = semctl(semid, 0, SETVAL, 1);
		p = semctl(semid, 1, SETVAL, 0);
		p = semctl(semid, 2, SETVAL, 0);


		// array of type sembuff operate on semaphore set
		struct sembuf sop[2];

		// attaching shared memory to the server address space
		msg = (message *)shmat(shmid, NULL, 0);

		while(1)
		{
		//  wait for readers turn
			printf("Trying to acquire semaphore.\n");
			sop[0].sem_num = 1;
			sop[0].sem_op = -1;
			sop[0].sem_flg = 0;

			// wait for semaphore access for shm
			sop[1].sem_num = 0;
			sop[1].sem_op = -1;
			sop[1].sem_flg = 0;

			if (semop(semid, (struct sembuf *)&sop, 2) == -1)
			{
				perror("Semop:\n");
			}

			//////////////////////// CRITICAL SECTION /////////////////////

			// Attaching shared memory
		
			//msg = (message *)shmat(shmid, NULL, 0);

		int j;
			for(j=0;j<10;j++){
	if(msgsCount[j].pid==(msg+(i*sizeof(message)))->pid){
	msgsCount[j].noOfMsgs+=1;
	break;
	}
	else if(msgsCount[j].pid==0){
	msgsCount[j].pid=(msg+(i*sizeof(message)))->pid;
	msgsCount[j].noOfMsgs=1;

	break;
	}
	}


			if ((msg+(i*sizeof(message)))->type == 1)
			{
				(msg+(i*sizeof(message)))->total = (msg+(i*sizeof(message)))->a + (msg+(i*sizeof(message)))->b;
				(msg+(i*sizeof(message)))->type =(msg+(i*sizeof(message)))->pid;
			}
			
			printf("Total is: %d\n", (msg+(i*sizeof(message)))->total);
	printf("\nserver:%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n",getpid(),i,(msg+(i*sizeof(message)))->a,(msg+(i*sizeof(message)))->b,(msg+(i*sizeof(message)))->total,shmid,semctl(semid,2,GETVAL));
		i++;
			printf("Total calculated.\n");
			/////////////////////// CRITICAL SECTION  /////////////////////

			// return for semaphore access for shm

			sop[0].sem_num = 0;
			sop[0].sem_op = 1;
			sop[0].sem_flg = 0;

			sop[1].sem_num = 2;
			sop[1].sem_op = 1;
			sop[1].sem_flg = 0;


			if (semop(semid, (struct sembuf *)&sop, 2) == -1)
			{
				perror("Semop:\n");
			}


		}
	}


	void int_handler1(int signo){
	int j;
	printf("\n");
	for(j=0;j<10;j++){
	if(msgsCount[j].pid==0)
	break;
	printf("pid:%d\t\tnoOfMsgs%d\n",msgsCount[j].pid,msgsCount[j].noOfMsgs);

	}
	exit(0);

	}



