
/** Assigned by: Sharon Bron-Sobol, 206171423 **/

typedef char list_type;

typedef struct song 
{
	char Name[20];
	char Artist[20];
   	struct song *next;
   	struct song *prev;
}song;

typedef struct song_list
{
	song *head;
	song *tail;
}game_list;

void initList(song_list *lst);

int isEmpty(song_list *lst);

int length(song_list *lst);

song* allocGame(char Name[20], char Artist[20]);

//song *getLinkBySerial(song_list *lst, int serial);

int insertFirst(song_list *lst, song *item);

int insertSong(song_list *lst, song *item);

int deleteFirst(song_list *lst);

int deleteLast(song_list *lst);

int deleteLink(song_list *lst, song *link);

int insertBefore(song_list *lst, song *item, song *link);
