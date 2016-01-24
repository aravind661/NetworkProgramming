/*
ALLAMNENI ARAVIND
(2012C6PS661P)
Undergrad @ BITS Pilani


The problem statement: 
Create a parent process P. P creates three processes A, B and C. Parent waits until all three exit. Consider shared memory segment of size 1KB created by process A and attached to processes A, B, C. These three processes want to access shared memory in the following ways:  
i. A reads from stdin and writes to shared memory. ii. B reads from shared memory, makes all lettrs to upper case and writes back into shared memory. iii. C reads from shared memory and prints to stdout.  
Processes A,B and C wants to access the shared memory segment in the order of steps i, ii, and iii and continue in loop until process A reads EOF from terminal.    
Each process should print its pid and the action they are doing.  Use semaphores for synchronizing. 

Files Expected: sem.c and makefile

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>

#define semkey (2012777)
//#define semkey2 (2012663)

void int_handler1(int signo);
void int_handler2(int signo);

pid_t parent, a=1, b=1, c=1;


int main(int argc, char *argv[]){
	int statA, statB, statC;
	//process ids of parent, A, B, C
	
	
	//shared memory key
	key_t key;
	
	// shared memory id
	int shmid;
	
	// semaphore id
	int semid;
	
	// creating pipe to share shmid
	int pfd[2];
	pipe(pfd);
	
	//data to be shared among processes A, B, C
	char *shm_address;
	
	// message withinin processes
	char message[100];
	
	// generating a key for shared momory
	key = ftok("assign.c", 'R');
	
	// creating a semaphore set
	semid = semget(semkey, 3, IPC_CREAT|0666);
	if(semid == -1){
		perror("Semapore set not created.\n");
	}
	if(semid > 0){
		printf("Semaphore set created.\n");
	}
	
	// control operations on a semaphore set
	// set one semaphore value to 1
	int p, q, r;
	p = semctl(semid, 0, SETVAL, 1);
	q = semctl(semid, 1, SETVAL, 0);
	r = semctl(semid, 2, SETVAL, 0);
	
	
	// create sembuff to operate on semaphore
	struct sembuff{
		unsigned short sem_num;
		short sem_op;
		short sem_flg;
	};
	
	struct sembuf sop[2];
	
	
	// parent creates a process A
	a = fork();
	
	// parent creates another child B
	if(a!=0){
		b = fork();
	}
	
	// declare the structure passed to the semop()
	//struct sembuf operations[2];
	//struct sembuf aravind;
	
	
	
	// parent creates another child C
	if(a!=0 && b!=0){
		c = fork();
	}
	
	// in parent process all a, b, c are non zero
	// the following code is executed in the parent process 
	if(a&&b&&c){
		// wait for all children to execute
		signal(SIGUSR1, int_handler1);
		waitpid(a, &statA, 0);
		waitpid(b, &statB, 0);
		waitpid(c, &statC, 0);
		//print pid and exit
		printf("All children of %d exited.\n", getpid());
		exit(1);
	}
	
	//printf("%d\n", getpid());
	// in process a, a==0
	//////////*********** PROCESS A **************/////////
	if(a==0){
		signal(SIGUSR2, int_handler2);
		//printf("I am %d the first child of %d\n", getpid(), getppid());
		
		// creating a shared memory
		shmid = shmget(key, 1024, 0644|IPC_CREAT);
		if(shmid == -1){
			perror("shmget: shmget failed");
			exit(1);
		}
		
		// attaching the shared memory
		shm_address = shmat (shmid, (void *) 0, 0);
		if (shm_address == (char *) (-1))
			perror ("shmat");
		
		// sending the shmid to other process
		write(pfd[1], &shmid, sizeof(shmid));
		//printf("successfully written shmid into pipe\n");
		
		while(1)
		{
		// wait(0)
		sop[0].sem_num = 0;
		sop[0].sem_op = -1;
		sop[0].sem_flg = 0;
	
		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		// reading from the stdin
		printf("Enter any string: \n");
		
		//scanf("%s", message);
		gets(shm_address);
		if(feof(stdin)){
			kill(getppid(), SIGUSR1);
		}
		
		// Process A has acquired semaphore
		printf("Process A has acquired shared memory.\n");
		
		
		//printf("%s", message);
		//strcpy(shm_address, message);
		printf("Process A wrote your statement to shared memory.\n");
		// Process A trying to release semaphore
				
		// giving turn to semaphore B (semaphore 1 to 1)
		
		// signal(1)
		sop[0].sem_num = 1;
		sop[0].sem_op = 1;
		sop[0].sem_flg = 0;

		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		}
	
		//printf("Process A: Now its your turn B .!!");
	}
	
	// in process b, b==0
	//////////********** PROCESS B ************////////////
	if(b==0){
		signal(SIGUSR2, int_handler2);
		//printf("I am %d the second child of %d\n", getpid(), getppid());

		// getting the shmid in B
		read(pfd[0], &shmid, sizeof(int *));
		// resending it for C
		write(pfd[1], &shmid, sizeof(shmid));
		//printf("Received in B : %d\n", shmid);
		
		
			
		// receiving message from shared memory
		//strcpy(message, shm_address);
		//sleep(2);
		//printf("This is process B\n");
		
		while(1)
		{
		
		// attaching the shared memory
		shm_address = shmat (shmid, (void *) 0, 0);
		if (shm_address == (char *) (-1))
			perror ("shmat");
		
		// wait(1)
		sop[0].sem_num = 1;
		sop[0].sem_op = -1;
		sop[0].sem_flg = 0;
		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		
		
		
		printf("Process B acquired shared memory.\n");
		printf("process B received: %s\n", shm_address);
		
		
		// converting to upper case
		/*char *p;
		
		
		*/
		//*shm_address = toupper(*shm_address);
		char *data;
		data = shm_address;
		//while(*shm_address++ = toupper(*shm_address));
		while(*data != '\0'){
			*data = toupper(*data);
			data++;
		}
		shm_address = data;
		
		
		// signal(1)
		sop[0].sem_num = 1;
		sop[0].sem_op = 1;
		sop[0].sem_flg = 0;
		// signal(2)
		sop[1].sem_num = 2;
		sop[1].sem_op = 1;
		sop[1].sem_flg = 0;
						
		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		if(semop(semid, &sop[1], 1) == -1){
			perror("semop");
			exit(1);
		}
		
		//printf("Process B: Now its your turn C .!!\n");
		
		}
		
	}
	
	// in process c, c==0
	//////////*********** PROCESS C *********/////////////
	if(c==0){
		
		signal(SIGUSR2, int_handler2);
		//printf("I am %d the third child of %d\n", getpid(), getppid());
		// getting the shmid in C
		read(pfd[0], &shmid, sizeof(int *));
		//printf("Received in C : %d\n", shmid);
		
		// attaching the shared memory
		shm_address = shmat (shmid, (void *) 0, 0);
		if (shm_address == (char *) (-1))
			perror ("shmat");
		//sleep(3);
		
		//printf("This is process C\n");
		while(1)
		{
		// wait(1)
		sop[1].sem_num = 1;
		sop[1].sem_op = -1;
		sop[1].sem_flg = 0;
		
		// wait(2)
		sop[0].sem_num = 2;
		sop[0].sem_op = -1;
		sop[0].sem_flg = 0;
		
		if(semop(semid, &sop[1], 1) == -1){
			perror("semop");
			exit(1);
		}
		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		
		printf("Process C acquired shared memory.\n");
		printf("process C received: %s\n", shm_address);
		
		
		// signal(0)
		sop[0].sem_num = 0;
		sop[0].sem_op = 1;
		sop[0].sem_flg = 0;
		
		if(semop(semid, &sop[0], 1) == -1){
			perror("semop");
			exit(1);
		}
		//printf("Process C: Now its your turn A .!!\n");
		}
		
	}
	
}

void int_handler1(int signo){
	kill(a,SIGUSR1);
	kill(b,SIGUSR1);
	kill(c,SIGUSR1);
}
void int_handler2(int signo){
exit(1);
}
