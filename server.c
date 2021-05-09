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
#include "list.h"

int clientCount = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
	struct song_list* songs;
};

struct client Client[1024];
pthread_t thread[1024];

void printMenu(char *songName){

}

void * doNetworking(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	printf("Client %d connected.\n",index + 1);
	printMenu(NULL);
	char *menu = (char*)malloc(100 * sizeof(char));
	sprintf(menu, "Now Playing :: %s\n========= MENU =========\n%s 			   :: Show song queue\n%s <SONG_NAME> :: Queue next song\n%s 			   :: Vote to skip song\n%s 			   :: Vote to pause song\n", LIST, PLAY, NEXT, PAUSE);
	// printf("Now Playing :: %s\n", songName);
	// printf("========= MENU =========\n");
	// printf("%s 			   :: Show song queue\n", LIST);
	// printf("%s <SONG_NAME> :: Queue next song\n", PLAY);
	// printf("%s 			   :: Vote to skip song\n", NEXT);
	// printf("%s 			   :: Vote to pause song\n", PAUSE);
	send(clientSocket,menu,1024,0);

	while(1){
		char data[1024];
		int read = recv(clientSocket,data,1024,0);//Wait for client choice
		data[read] = '\0';

		char output[1024];
		if(!strncmp(data, PLAY, 5)){
			insertFirst(clientDetail->songs, allocSong(clientDetail->index, data, data));
		}
		if(strcmp(data,"LIST") == 0){

			int l = 0;

			for(int i = 0 ; i < clientCount ; i ++){

				if(i != index)
					l += snprintf(output + l,1024,"Client %d is at socket %d.\n",i + 1,Client[i].sockID);

			}

			send(clientSocket,output,1024,0);
			continue;

		}
		if(strcmp(data,"SEND") == 0){

			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			int id = atoi(data) - 1;

			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			send(Client[id].sockID,data,1024,0);			

		}

	}

	return NULL;

}

void main(){
	struct song_list* songs = (struct song_list*) malloc(sizeof(struct song_list)); //Song Queue
	initList(songs); //Init queue

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1)
		perror("Error : Bind");

	if(listen(serverSocket, MAX_CONN) == -1) 
		perror("Error : Listen");

	printf("Server started listenting on port 8080 ...........\n");

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);

		clientCount ++;
 
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}