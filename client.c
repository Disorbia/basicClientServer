#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "defs.h"

void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
		if(read == 0){
			printf("Server disconnected\n");
			exit(0);
		}
		printf("%s\n",data);
	}
}

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("usage is: ./client <ip_address> <port>\n");
		exit(0);
	}
	int PORT = atoi(argv[2]);

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
		if(!strcmp(argv[1], "localhost"))
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
 	else
		serverAddr.sin_addr.s_addr = inet_addr(argv[1]);


	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");

	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	while(1){

		char input[1024];			
		scanf("%s",input);
		send(clientSocket,input,1024,0);

		if(!strcmp(input, QUIT)){
			shutdown(clientSocket, 0);
		}
	}


}