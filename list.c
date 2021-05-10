#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/** Assigned by: Sharon Bron-Sobol, 206171423 **/

void initList(song_list *lst)
{
	lst->head = lst->tail = NULL;	
}

//is list empty
int isEmpty(song_list *lst)
{
	return lst->head == NULL;
}

//return amount of games in the list
int length(song_list *lst)
{
	int length = 0;
	song *current;
	
	for(current = lst->head; current != NULL; current = current->next)
		length++;
	
   return length;
}

//allocate a new song
song* allocSong(int User, char Name[40])
{
	song* item = (song*) malloc(sizeof(song));
	strncpy(item->Name, Name, 40);

	item->next = NULL;
	item->prev = NULL;

	return item;
}

//insert item at the first location
int insertLast(song_list *lst, song *item)
{
	if (item != NULL)
	{
		//reset item pointers
		item->next = item->prev = NULL;
	
		if(isEmpty(lst))
		{
			//make head & tail pointing to item
			lst->head = lst->tail = item;	
		}
		else 
		{
			lst->tail->next = item;
			item->prev = lst->tail;
			item->next = lst->head;
			lst->tail = item;
			//update first prev link
			//lst->head->prev = item;
			//point item to old first link
			//item->next = lst->head;
			//point head to new first song
			//lst->head = item;
		}
		return 1;
	}
	return 0;
}

//insert item at the appropriate location (sorted by serial)
/*int insertSong(struct song_list *lst, struct song *item)
{
	struct song *p = lst->head;

	if(isEmpty(lst))
	{
		//make head & tail pointing to item
		lst->head = lst->tail = item;
	}
	else{
		while(p != NULL){
			if(p->serial >= item->serial){ // search for the first item bigger than this one
				insertBefore(lst, item, p); //insert before the bigger item (insert with sorting)
				return 1;
			}
			p = p->next;	
		}
		//if inserting at tail:
		lst->tail->next = item;     
		//point item to old last link
		item->prev = lst->tail;
		//point last to new last user
		lst->tail = item;
	}
	return 0;
}*/

//delete first item
int deleteFirst(song_list *lst)
{	
	//if list is empty there is nothing to delete
	if (!isEmpty(lst))
	{
		//save reference to first link
		song *temp = lst->head;

		//if only one link
		if(temp->next == NULL)
		{			
			initList(lst);
		}
		else
		{
			lst->head->next->prev = NULL;
			lst->head = lst->head->next;
		}
		free(temp);
		return 1;
	}
	return 0;
}
 
//delete link at the last location
int deleteLast(song_list *lst)
{
	//if list is empty there is nothing to delete
	if (!isEmpty(lst))
	{
		//save reference to last link
		song *temp = lst->tail;

		//if only one link
		if(temp->prev == NULL)
		{
			initList(lst);
		}
		else
		{
			lst->tail->prev->next = NULL;
			lst->tail = lst->tail->prev;
		}
		free(temp);
		return 1;
	}
	return 0;
}

//delete link from list
int deleteLink(song_list *lst, song *link)
{
	if (!isEmpty(lst))
	{
		if (link == NULL)
			return 0;
		if (link == lst->head)
			return deleteFirst(lst);
		if (link == lst->tail)
			return deleteLast(lst);

		//bypass the current link
		link->prev->next = link->next;
		link->next->prev = link->prev;
		free(link);
		return 1;
	}
	return 0;
}

//insert a new item before link
/*int insertBefore(song_list *lst, song *item, song *link)
{

	if (!isEmpty(lst))
	{
		if ((item == NULL) || (link == NULL))
			return 0;
		if (link == lst->head)
			return insertFirst(lst,item);

		//assign new item pointers
		item->next = link;
		item->prev = link->prev;
		//chain link and link->prev to new item
		link->prev->next = item;
		link->prev = item;
		return 1;
	}
	return 0;
}*/