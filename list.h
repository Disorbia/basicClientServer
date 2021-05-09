
/** Assigned by: Sharon Bron-Sobol, 206171423 **/

typedef char list_type;

struct song 
{
	int User;
	char Name[20];
	char Artist[20];
   	struct song *next;
   	struct song *prev;
};

struct song_list
{
	struct song *head;
	struct song *tail;
};

void initList(struct song_list *lst);

int isEmpty(struct song_list *lst);

int length(struct song_list *lst);

struct song* allocSong(int User, char Name[20], char Artist[20]);

//song *getLinkBySerial(song_list *lst, int serial);

int insertFirst(struct song_list *lst, struct song *item);

//int insertSong(struct song_list *lst, struct song *item);

int deleteFirst(struct song_list *lst);

int deleteLast(struct song_list *lst);

int deleteLink(struct song_list *lst, struct song *link);

int insertBefore(struct song_list *lst, struct song *item, struct song *link);
