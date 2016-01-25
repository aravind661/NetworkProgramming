/*

ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani

Problem Statement: 
Write a C program multicast.c for IP Multicast that does the following.   
1) It takes multicast group ip and port on command line. It joins the group and waits for messages.
2) It sends a "hello-" + time() every 15 seconds to count how many members are present in the group.
3) Any member who receives "hello-"+time(), simply echoes the same.
4) The member which has sent hello, counts the replies received within 5 seconds and displays count on the screen.
5) Same program is run multiple times to create multiple members.
6) Each member prints the sender ip and message received every time a message is received. 
7) member exits only when Ctrl-c is pressed. Before it exits, it sends "bye-" +time() to all in the group. 

*/

#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#define EXAMPLE_PORT 6005
#define EXAMPLE_GROUP "172.17.20.1"

void int_handler1(int signo);

main(int argc, char **argv[])
{

signal(SIGINT, int_handler1);
   struct sockaddr_in addr;
   int addrlen, sock, cnt;
   struct ip_mreq mreq;
   char message[50];

   /* set up socket */
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
     perror("socket");
     exit(1);
   }
   bzero((char *)&addr, sizeof(addr));
   addr.sin_family = AF_INET;
 //addr.sin_addr.s_addr = htonl(INADDR_ANY);
   addr.sin_addr.s_addr =htonl(argv[1]);
 //addr.sin_port = htons(EXAMPLE_PORT);
   addr.sin_port =atoi(argv[2]);
   addrlen = sizeof(addr);

   if (argc > 3) {
      /* send */
      addr.sin_addr.s_addr = inet_addr(EXAMPLE_GROUP);
      while (1) {
	 time_t t = time(0);
	 sprintf(message, "time is %-24.24s", ctime(&t));
	 printf("sending: %s\n", message);
	 cnt = sendto(sock, message, sizeof(message), 0,
		      (struct sockaddr *) &addr, addrlen);
	 if (cnt < 0) {
 	    perror("sendto");
	    exit(1);
	 }
	 sleep(5);
      }
   } else {

      /* receive */
      if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {        
         perror("bind");
	 exit(1);
      }    
      mreq.imr_multiaddr.s_addr = inet_addr(EXAMPLE_GROUP);         
      mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		     &mreq, sizeof(mreq)) < 0) {
	 perror("setsockopt mreq");
	 exit(1);
      }         
      while (1) {
 	 cnt = recvfrom(sock, message, sizeof(message), 0, 
			(struct sockaddr *) &addr, &addrlen);
	 if (cnt < 0) {
	    perror("recvfrom");
	    exit(1);
	 } else if (cnt == 0) {
 	    break;
	 }
	 printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
        }
    }
}


void int_handler1(int signo){

//send bye +time to all connected users

exit(1);

}
