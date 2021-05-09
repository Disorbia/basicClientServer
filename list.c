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
song *allocGame(char Name[20], char Artist[20])
{
	song* item = (song*)malloc(sizeof(song));
	item->serial = serial;
	strncpy(item->Name, Name, 20);
	strncpy(item->Artist, Artist, 20);

	item->next = NULL;
	item->prev = NULL;

	return item;
}
//return item by serial property
/*song *getLinkBySerial(song_list *lst, int serial){
	song *p = lst->head;
	while(p != NULL){
		if(p->serial == serial)
			return p;
		p = p->next;
	}
	return NULL;
}*/


//insert item at the first location
int insertFirstG(song_list *lst, song *item)
{
	if (item != NULL)
	{
		//reset item pointers
		item->next = item->prev = NULL;
	
		if(isEmptyG(lst))
		{
			//make head & tail pointing to item
			lst->head = lst->tail = item;	
		}
		else 
		{
			//update first prev link
			lst->head->prev = item;
			//point item to old first link
			item->next = lst->head;
			//point head to new first song
			lst->head = item;
		}
		return 1;
	}
	return 0;
}

//insert item at the appropriate location (sorted by serial)
int insertGame(song_list *lst, song *item)
{
	song *p = lst->head;

	if(isEmptyG(lst))
	{
		//make head & tail pointing to item
		lst->head = lst->tail = item;
	}
	else{
		while(p != NULL){
			if(p->serial >= item->serial){ // search for the first item bigger than this one
				insertBeforeG(lst, item, p); //insert before the bigger item (insert with sorting)
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
}

//delete first item
int deleteFirstG(song_list *lst)
{	
	//if list is empty there is nothing to delete
	if (!isEmptyG(lst))
	{
		//save reference to first link
		song *temp = lst->head;

		//if only one link
		if(temp->next == NULL)
		{			
			initListG(lst);
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
int deleteLastG(song_list *lst)
{
	//if list is empty there is nothing to delete
	if (!isEmptyG(lst))
	{
		//save reference to last link
		song *temp = lst->tail;

		//if only one link
		if(temp->prev == NULL)
		{
			initListG(lst);
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
int deleteLinkG(song_list *lst, song *link)
{
	if (!isEmptyG(lst))
	{
		if (link == NULL)
			return 0;
		if (link == lst->head)
			return deleteFirstG(lst);
		if (link == lst->tail)
			return deleteLastG(lst);

		//bypass the current link
		link->prev->next = link->next;
		link->next->prev = link->prev;
		free(link);
		return 1;
	}
	return 0;
}

//insert a new item before link
int insertBeforeG(song_list *lst, song *item, song *link)
{

	if (!isEmptyG(lst))
	{
		if ((item == NULL) || (link == NULL))
			return 0;
		if (link == lst->head)
			return insertFirstG(lst,item);

		//assign new item pointers
		item->next = link;
		item->prev = link->prev;
		//chain link and link->prev to new item
		link->prev->next = item;
		link->prev = item;
		return 1;
	}
	return 0;
}