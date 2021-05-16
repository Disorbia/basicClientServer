#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
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
FILE *queue_file;
pthread_t thread[1024];
pthread_t player;

char * getNowPlaying(song_list *songs){
	char *nowMsg = (char*) malloc(60 * sizeof(char));
	if(!strcmp(songs->nowPlaying->Name ,null))
		return "None";
	sprintf(nowMsg, "%s by User %d\n", songs->nowPlaying->Name, songs->nowPlaying->User);
	return nowMsg;
}

void sendSongList(int clientSocket, song_list *songs){
	song *p = songs->head;
	send(clientSocket,"\n=======SONG QUEUE=======\n",1024,0);
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
	send(clientSocket,"\n========================\n",1024,0);

}

void * songPlayer(void * songs_l){
	struct song_list* songs_queue =(song_list*) songs_l;
	int timer = 0;
		while(1){
		int play = songs_queue->play;
			sleep(1);
			if(songs_queue->nowPlaying == songs_queue->tail && !strcmp(songs_queue->nowPlaying->Name ,null)){ //Waiting for a new queue
				timer = SONG_TIME;
				continue;
			}
			if(timer == SONG_TIME || songs_queue->skip){
				if(songs_queue->nowPlaying == songs_queue->tail){
					insertNULL(songs_queue);
					fprintf(queue_file, "No Song\n");
				}
				else{
					songs_queue->nowPlaying = songs_queue->nowPlaying->next;
					fprintf(queue_file, "%s by User %d\n", songs_queue->nowPlaying->Name, songs_queue->nowPlaying->User);

				}
				timer = 0;
				songs_queue->skip = 0;
			}
			else if(play)
				timer++;
		}
}

void SIGINIT_handler(){
	fclose(queue_file);
	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i], NULL);
	pthread_kill(player, SIGINT);
	printf("\nGoodbye!\n");
	exit(0);

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
		sprintf(menu, "Now Playing :: %s\n========= MENU =========\n%s 			   	  :: Show song queue\n%s <SONG_NAME>-<ARTIST_NAME> :: Queue next song\n%s 			   	  :: Play song\n%s 			   	  :: Pause song\n%s 			   	  :: Skip song\n========================\n",nowPlaying, LIST, QUEUE, PLAY, PAUSE, NEXT);
		send(clientSocket,menu,1024,0);

		char data[1024];
		int read = recv(clientSocket,data,1024,0);//Wait for client choice		
		data[read] = '\0';
		if(!strncmp(data, QUEUE, 5)){
			read = recv(clientSocket,data,1024,0);//Song name
			data[read] = '\0';
			insertLast(clientDetail->songs, allocSong(clientDetail->index, data));
		}
		else if(!strncmp(data,PLAY, 5)){
			clientDetail->songs->play = 1;
		}
		else if(!strncmp(data,LIST, 5)){
			sendSongList(clientSocket, clientDetail->songs);
		}
		else if(!strncmp(data, PAUSE, 5)){
			clientDetail->songs->play = 0;
		}
		else if(!strncmp(data, NEXT, 5)){
			clientDetail->songs->skip = 1;
		}
		// 	int id = atoi(data) - 1;
		}
	}


void main(int argc, char *argv[]){
	if(argc < 3){
		printf("usage is: ./server <ip_address> <port>\n");
		exit(0);
	}
	int PORT = atoi(argv[2]);

	song_list *songs_l = (song_list*) malloc(sizeof(song_list)); //Song Queue
	queue_file = fopen("queue.txt", "a");
	initList(songs_l); //Init queue
	insertNULL(songs_l); //Insert an empty song

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	if(!strcmp(argv[1], "localhost"))
		serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
 	else
		serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	struct sigaction sa_pipe;
    sa_pipe.sa_handler = SIG_IGN;
    sa_pipe.sa_flags = 0;
    sigemptyset(&sa_pipe.sa_mask);//SIGPIPE

 	signal(SIGINT, SIGINIT_handler);//SIGINT

	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1)
		perror("Error : Bind");

	if(listen(serverSocket, MAX_CONN) == -1) 
		perror("Error : Listen");

	pthread_create(&player, NULL, songPlayer, (void *) songs_l);

	printf("Server started listenting on port %d ...........\n", PORT);

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;
		Client[clientCount].songs = songs_l;
	    if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1) {
	        perror("SIGPIPE");
    	    exit(1);
    	}

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);
		clientCount ++;
 
	}

}