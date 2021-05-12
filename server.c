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
	song_list *songs;
};

struct client Client[1024];
pthread_t thread[1024];
pthread_t player;


char * getNowPlaying(song_list *songs){//TODO: Add real cycling of songs
	if(!strcmp(songs->nowPlaying->Name ,null))
		return "None";
	return songs->nowPlaying->Name;
}

void sendSongList(int clientSocket, song_list *songs){
	song *p = songs->head;
	if(p != NULL){
		while(1){
			if(!strcmp(p->Name, null))
				send(clientSocket,"None",1024,0);
			else
				send(clientSocket,p->Name,1024,0);
			if(p == songs->tail)
				break;
			p = p->next;
		}
	}
}

void * songPlayer(void * songs_l){
	struct song_list* songs_queue =(song_list*) songs_l;
	int timer = 0;
		while(1){
			sleep(1);
			if(songs_queue->nowPlaying == songs_queue->tail && !strcmp(songs_queue->nowPlaying->Name ,null)){ //Waiting for a new queue
				timer = 7;
				continue;
			}
			if(timer == 7){
				if(songs_queue->nowPlaying == songs_queue->tail)
					insertNULL(songs_queue);
				else
					songs_queue->nowPlaying = songs_queue->nowPlaying->next;

				timer = 0;
			}
			else
				timer++;
		}
}

void * doNetworking(void * ClientDetail){
	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
	printf("Client %d connected.\n",index + 1);
	char *nowPlaying = (char*) malloc(30 * sizeof(char));

	while(1){
		nowPlaying = getNowPlaying(clientDetail->songs);
		char *menu = (char*)malloc(500 * sizeof(char));	
		sprintf(menu, "Now Playing :: %s\n========= MENU =========\n%s 			   :: Show song queue\n%s <SONG_NAME>-<ARTIST_NAME>:: Queue next song\n%s 			   :: Vote to skip song\n%s 			   :: Vote to pause song\n",nowPlaying, LIST, PLAY, NEXT, PAUSE);
		send(clientSocket,menu,1024,0);

		char data[1024];
		int read = recv(clientSocket,data,1024,0);//Wait for client choice		
		data[read] = '\0';

		if(!strncmp(data, PLAY, 5)){
			read = recv(clientSocket,data,40,0);//Song name
			data[read] = '\0';
			insertLast(clientDetail->songs, allocSong(clientDetail->index, data));
			sleep(1);
		}
		if(!strncmp(data,LIST, 5)){
			sendSongList(clientSocket, clientDetail->songs);
		}

		// if(strcmp(data,"SEND") == 0){

		// 	read = recv(clientSocket,data,1024,0);
		// 	data[read] = '\0';

		// 	int id = atoi(data) - 1;

		// 	read = recv(clientSocket,data,1024,0);
		// 	data[read] = '\0';

		// 	send(Client[id].sockID,data,1024,0);			
		// }

		}
	}


void main(){
	song_list *songs_l = (song_list*) malloc(sizeof(song_list)); //Song Queue
	//song* t_song = (song*) malloc(sizeof(song));

	initList(songs_l); //Init queue
	insertNULL(songs_l); //Insert an empty song

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1)
		perror("Error : Bind");

	if(listen(serverSocket, MAX_CONN) == -1) 
		perror("Error : Listen");

	pthread_create(&player, NULL, songPlayer, (void *) songs_l);

	printf("Server started listenting on port 8080 ...........\n");

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;
		Client[clientCount].songs = songs_l;

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);

		clientCount ++;
 
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);
	pthread_join(player, NULL);

}