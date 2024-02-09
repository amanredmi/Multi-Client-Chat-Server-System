// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<stdlib.h>
#include<signal.h>
#define PORT 8080
int c;
void sigCHandler() 
{
	
	
	exit(1);

} 

/*Function to handle ^Z*/
void sigZhandler() 
{ 
	
	
	exit(1);

}
int main(int argc, char const* argv[])
{
	
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
    char *actual;
	char hello [256];
	//printf("enter operation you want to perform:\n");
	//scanf("%[^\n]%*c",hello);
	char buffer[256] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
	c=client_fd;
	signal(SIGINT, sigCHandler);  // handles ^C
	signal(SIGTSTP, sigZhandler);    //handles ^Z
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status
		= connect(client_fd, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
    valread = read(client_fd, buffer, 256);
    printf("%s\n",buffer);
    if(strcmp(buffer,"connection limit exceeded!!")==0)
        return 1;
    if(fork()==0)
	{
		printf("enter command\n");
		fgets(hello, 256, stdin);
		hello[strcspn(hello, "\n")] = 0;

		while(strcmp(hello,"/quit")!=0)
		{
			//printf("scanned string is %s\n",hello);
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,hello);
			//printf("sending %s\n",buffer);
        	send(client_fd,buffer,strlen(buffer),0);
			
			//printf("&\n");
			memset(hello,'\0',sizeof(hello));
		//fflush(stdin);
			fgets(hello, 256, stdin);
			hello[strcspn(hello, "\n")] = 0;
			//printf("scanned string is %s\n",hello);

		}
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,hello);
		send(client_fd,buffer,strlen(buffer),0);
		close(client_fd);

	}
	else
	{
		while(strcmp(buffer,"connection terminated")!=0)
		{
			memset(buffer,'\0',sizeof(buffer));
			recv(client_fd,buffer,sizeof(buffer),0);
			printf("%s\n",buffer);
			
		}
		close(client_fd);
	
	}

    
	
	// closing the connected socket
	
	return 0;
}