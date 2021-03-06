#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>	   //This header file contains definitions of a number of data types used in system calls
#include <sys/socket.h>    //The header file socket.h includes a number of definitions of structures needed for sockets.
#include <netinet/in.h>    //The header file in.h contains constants and structures needed for internet domain addresses.
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

//structure defining the contents of each message.
typedef struct im_message {
  char from[256];
  char message[1024];
} im_message;

//structure containing the registered users with their ports and address
typedef struct user_log {
  struct sockaddr_in userport;
  char uname[256];
} users;

//This function is called when a system call fails. It displays a message about the error on stderr and then aborts the program.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
users det[100];
   int sock, length, n;		    //sock store the values returned by the socket system call , 
   socklen_t fromlen;			//stores the size of the sockaddr structuree
   struct sockaddr_in server;   //A sockaddr_in is a structure containing an internet address. This structure is defined in netinet/in.h.
   struct sockaddr_in from;
  // char buf[1024];
   im_message user_message;	//struct type im_message which contains the to from message information for client side
   im_message server_message;	//defined for server side
   int count = 0;		//variable which keeps the track of no of users in det[] array

//The user needs to pass in the port number on which the server will accept connections as an argument,displays an error message if the user fails to do this.
   if (argc < 2) 
   {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);  //The socket() system call creates a new socket,takes three arguments,The first is the address domain of the socket.
   if (sock < 0) 
	error("Opening socket");

   length = sizeof(server);
   bzero(&server,length);   				//sets all values in a buffer to zero,first arg is a pointer to the buffer and the second arg is the size of the buffer
   server.sin_family=AF_INET;				//The variable serv_addr is a structure of type struct sockaddr_in,which contains a code for the address family.
   server.sin_addr.s_addr=INADDR_ANY;		//This field contains the IP address of the host
   server.sin_port=htons(atoi(argv[1]));	//The port number on which the server will listen for connections 

   if (bind(sock,(struct sockaddr *)&server,length)<0)  // binds a socket to an address, the address of the current host and port number on which the server will run
       error("binding");
   fromlen = sizeof(struct sockaddr_in);

 //  int p ;
//never ending seerver loop, ends using ctrl+c
   while (1) 
   {
        n = recvfrom(sock,&user_message,sizeof(im_message),0,(struct sockaddr *)&from,&fromlen); //explained at the top of the file
		if (n < 0)
			error("recvfrom");


		int j,i=0;
	
		int k=0;
		char spr[1024],new_str[1024];
		
		if(user_message.message[0]==':')
		{
			for(i=0;i<count;i++)
			{
				if(!strcmp(user_message.from,det[i].uname))
				{
					break;		//breaks the loop as soon as it finds the name
				}
			}
			if(i==count)
			{
				strcpy(det[count].uname,user_message.from);
				det[count].userport=from;
				printf("%s\t%d\tRegistered\n",det[count].uname,det[j].userport.sin_port);
				count++;
				strcpy(server_message.message,"Successful");
			}
			else
			{
				strcpy(server_message.message,"Unsuccessful");
			}
			n = sendto(sock,&server_message,sizeof(im_message),0,(struct sockaddr *)&from,fromlen);
			continue;
		}
		

		strcpy(server_message.message,user_message.message);	 		//extracting the message from the client message removing the other clients name
		strcpy(server_message.from,user_message.from); 	//copy the from of client to server message
		
		for(j=0; j<count; j++)
		{
			//printf("%s \t %d\n",det[j].uname,det[j].userport);
			if(strcmp(det[j].uname,server_message.from))
				n = sendto(sock,&server_message,sizeof(im_message),0,(struct sockaddr *)&(det[j].userport),fromlen);
		}
		
			//sends to reciever/client mentioning the port in 5th argument
		
   }
   
   return 0;
}

