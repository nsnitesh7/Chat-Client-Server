#include <string.h>
#include <stdlib.h>
#include <sys/types.h>     //This header file contains definitions of a number of data types used in system calls
#include <sys/socket.h>    //The header file socket.h includes a number of definitions of structures needed for sockets.
#include <netinet/in.h>    //The header file in.h contains constants and structures needed for internet domain addresses.
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


//structure defining the contents of each message.
typedef struct im_message {
  char from[256];
  char message[1024];
} im_message;

//This function is called when a system call fails. It displays a message about the error on stderr and then aborts the program.
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc,char *argv[])
{
	int sock, n;						//sock store the values returned by the socket system call , 
	unsigned int length;				//stores the size of the sockaddr structuree
   	struct sockaddr_in server, from;	//A sockaddr_in is a structure containing an internet address. This structure is defined in netinet/in.h.
   	char buffer[256];		  			//stores the clients message from stdin 
	char clients_name[256];			    //stores the name of the user from command line argument
	im_message client_message;			//struct type im_message which contains the to from message information for client side
    im_message server_message;			//defined for server side
   
	if (argc != 4)						//displays the error if user gives less than 4 arguments 
	{ 
		printf("Usage: ./a.out username server port\n");
        exit(1);
    }

	fd_set readfds;			// Initialize the file descriptor set to include our socket  
  	int rval;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);   //The socket() system call creates a new socket,takes three arguments,The first is the address domain of the socket.
	if (sock < 0) error("socket");
	strcpy(clients_name,argv[1]);
	server.sin_family = AF_INET;				//The variable serv_addr is a structure of type struct sockaddr_in,which contains a code for the address family.
	inet_aton(argv[2],&server.sin_addr);
	server.sin_port = htons(atoi(argv[3]));		//The port number on which the server will listen for connections 
	length=sizeof(struct sockaddr_in);			
	strcpy(client_message.message,":");
	strcpy(client_message.from,clients_name);		//copies the name of user to from field of structure
	n = sendto(sock,&client_message,sizeof(im_message),0,(const struct sockaddr *)&server, length);	//sends the message to server for registration
	n = recvfrom(sock,&server_message,sizeof(im_message),0,(struct sockaddr *)&from, &length);		//receives back from server 
	
	if(!strcmp(server_message.message,"Successful"))			//if the registration is successfull
	{
		while(1)						//while will keep on running untill the user exits
		{	
			printf("imc>");			
			fflush(stdout);
			bzero(buffer,256);			//initialise the buffer array to zero
			FD_CLR(sock, &readfds);		//Remove fd from the set.
  			FD_CLR(0, &readfds);
  			FD_ZERO(&readfds);			//Clear all entries from the set.
  			FD_SET(sock, &readfds);		//Add fd to the set.
  			FD_SET(0, &readfds);
			
//The select() function is designed to wait for input from multiple sources at once, and let you process whichever input actually arrives. 			
			rval = select(sock+1, &readfds, NULL, NULL, NULL);
			
/* We have socket input waiting, and can now do a recvfrom(). */
            if ((rval == 1) && (FD_ISSET(sock, &readfds)))			//Return true if fd is in the set.
			{
				n = recvfrom(sock,&server_message,sizeof(im_message),0,(struct sockaddr *)&from, &length);
				if(n>=0)
				printf("%s Says : %s",server_message.from,server_message.message);	
            } 
			if ((rval == 1) && (FD_ISSET(0, &readfds))) 
			{
/* We have input waiting on standard input, so we can go get it. */
				fgets(buffer,255,stdin);
				
				strcpy(client_message.from,clients_name);
				int i=0;
				int len;
				strcpy(client_message.message,buffer);		//copies the message to message field 

				n = sendto(sock,&client_message,sizeof(im_message),0,(const struct sockaddr *)&server, length); //sends the message to server 
			} 					
		}
	}
	else if(!strcmp(server_message.message,"Unsuccessful"))
	{
		printf("client already exists\n");
	}

return 0;
}

