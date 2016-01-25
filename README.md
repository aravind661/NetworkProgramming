# RSHM API
In this problem, goal is to facilitate two processes on two different systems use shared memory for IPC as if they are on the same system as shown in the diagram below.    
1) P1 and P2 will use shared memory for inter process communication. But they use the following API instead of usual System V shared memory API. 
            int rshmget(key_t key, size_t size);
            void rshmat(int rshmid, void* addr);
            int rshmdt(int rshmid, void* addr);
            int rshmctl(int rshmid, int cmd);
            void rshmChanged(int rshmid); 
# rshmget():            
API :  it prepares a message with appropriate contents and send to local TCP server. 
TCP Server : It checks the key. If it already exists it returns the rshmid otherwise, it creates shared memory locally and stores the shmid. Generates the rshmid, a random number, and sends a message to other nodes about the new shared memory creation. Returns the rshmid. This node becomes the owner of the shared memory.

# rshmat():
API: it prepares a message with appropriate contents and send to local TCP server. 
TCP Server: It calls shmat() and returns the return address of shmat(). Sends a message to other nodes to increase ref count. 
Remote TCP Servers: Increase ref count for the corresponding shm segment

# rshmdt():
API: it prepares a message with appropriate contents and send to local TCP server. 
TCP Server: It calls the shmdt() and sends message to other nodes to decrease ref count.  
Remote TCP Server: Decrease ref count for the corresponding shm segment

# rshmctl():
API: only command applicable is IPC_RMID. it prepares a message with appropriate contents and send to local TCP server. 
TCP Server: It calls the shmctl() and informs the same to other nodes using multicast message
Rempte TCP Servers: Remove the shared memory corresponding to rshmid that is received from the owner

# rshmChanged():
API: After completing the write operation, the application calls rshmChanged().it prepares a message with appropriate contents and        send to local TCP server.
TCP Server: The server reads the data from shared memory and sends to all other nodes to update their local shared memory segments. 
Remote TCP Server: Update the local shared memory corresponding to rshmid received.

TCP Servers: When the server starts up, it sends hello message to the group. It rears the replies and updates its internal state tables. 
Rempte TCP Server: Reply with the local state table 

2) TCP server stores the following information for every shared memory segment request it receives.
            struct rshminfo{ 
                  int rshmid;           /*unique id across all systems. created by the first system*/ 
                  key_t key;            /*key used to create shm segment*/
                  int shmid;            /*shmid returned by the local system*/
                  void *addr;           /*address returned by the local system*/
                  int ref_count;        /*no of processes attached to*/ 
                  struct sockaddr_in *remote_addrs; /* list of remote end points*/
            };

3) TCP server maintains connections to all remote TCP servers. It uses message queues for communicating with local system processes and TCP connections to communicate with remote systems.  

Implement client.c (run as P1 or P2), rshmAPI.c (implementation of API) and rshmServer.c (run as TCP server) for the above specifications. client.c uses rshmAPI.c.
