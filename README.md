# SHM Client & SHM Server
Clients and server that interact through shared memory

*open in editor mode to understand the below illustration better*

Programs shmclient.c and shmserver.c that do the following
             ___________
c1---------->| S    M  |         ____________
             | H    E  |         |          |
             | R    M  |-------->|  Server  |
c2---------->| E    O  |         |          |
             | D    R  |         ````````````
c3---------->|      Y  |
             ```````````
             
1) Server creates a shared memory of 1 MB. Shared memory is protected by a semaphore S. Clients write messages into shared memory. Server sums up a and b and put the result into total.

2)  Message is of the format:
            struct message{
                        int type;
                        int pid; //client's pid
                        int slno; //incremented for every message
                        int a; //any number
                        int b; //any number
                        int total;//total of a and b, processed by server
            }; 
            
3) A client writes a message m of type 1 into shared memory by acquiring sizeof(m) bytes from S. A client can write several messages into shared memory at one time. But as soon as a client completes writing, control is given to server to read a message from the shared memory.

4) Server reads one message at a time, processes and writes the reply with type of client's pid into the shared memory after acquiring necessary bytes from S. Once the server writes the reply, control is given to the client, who needs to read that reply. Such a client can't write but only read from shared memory. After the client completes reading, either server (for reading) or any client (for writing) can get chance to access shared memory.

Use semaphores wherever necessary to control access to shared memory. Print client’s pid, slno, a and b, shmid, semaphore value for every message the client puts in. Print server’s pid (with label “server”),  slno, a, b, sum of a and b,  shmid, and semaphore value for every message it processes. When server is exited through ctrl-c, it prints pid wise count of messages it processed. 
