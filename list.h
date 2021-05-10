
/** Assigned by: Sharon Bron-Sobol, 206171423 **/

typedef struct song 
{
	int User;
	char Name[40];
   	struct song *next;
   	struct song *prev;
}song;

typedef struct song_list
{
	song *head;
	song *tail;
}song_list;

void initList(song_list *lst);

int isEmpty(song_list *lst);

int length( song_list *lst);

 song* allocSong(int User, char Name[40]);

//song *getLinkBySerial(song_list *lst, int serial);

int insertLast(song_list *lst, song *item);

//int insertSong(struct song_list *lst, struct song *item);

int deleteFirst(song_list *lst);

int deleteLast(song_list *lst);

int deleteLink(song_list *lst, song *link);

int insertBefore(song_list *lst, song *item, song *link);
