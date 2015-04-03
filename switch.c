/* File: switch.c
 * Name: Ryan Walser
 * Group: Michele Shimoda, Scott Nakashima
 */

/* Libraries and Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include "utilities.h"
#include "link.h"
#include "man.h"
#include "host.h"
#include "switch.h"

/* Unique Macros */
#define EMPTY_ADDR 0xffff
#define MAXBUFFER 1000

/* Queue */
const packetBuffer BUFFERROR = { .srcaddr = -1, .dstaddr = -1, .length = -1,
	.valid = -1, .start = -1, .end = -1};

void initQ(Queue *q)
{
	q->head = 0;
	q->tail = -1;
	q->size = 0;
}
void push(Queue *q, packetBuffer data)
{
	/* Check if full */
	if(q->size >= 10000)
	{
		printf("Error: Queue is full.\n");
		return;
	}

	/* Push */
	q->tail++;
	q->tail %= 10000;
	q->elements[q->tail] = data;
	q->size++;
}
packetBuffer pop(Queue *q)
{
	/* Save data */
	packetBuffer data = q->elements[q->head];

	/* Check if empty */
	if(q->size == 0)
	{
		printf("Error: Queue is empty.\n");
		return BUFFERROR;
	}

	/* Pop */
	q->head++;
	q->head %= 10000;
	q->size--;
	return data;
}

/* Table */
void InitTable(FTable *table)
{
	table->size = 0;
}

void UpdateT(FTable *table, int valid, int D_addr, int OlinkID)
{
	/* Find index */
	int index = IndexT(table, D_addr);
	if(index == ERROR)
	{
		AddT(table, valid, D_addr, OlinkID);
	}
	else
	{
		UpdateIndexT(table, index, valid, OlinkID);
	}
}

int Olink(FTable *table, int D_addr)
{
	/* Find index */
	int index = IndexT(table, D_addr);
	
	/* If Error */
	if(index == ERROR)
	{
		return ERROR;
	}

	return table->entry[index].OlinkID;
}

void AddT(FTable *table, int valid, int D_addr, int OlinkID)
{
	/* New entry */
	FTEntry new;
	
	/* Gather info */
	new.D_addr = D_addr;
	new.OlinkID = OlinkID;
	new.valid = valid;

	/* Add to table */
	table->entry[table->size] = new;
	table->size++;
}

int IndexT(FTable *table, int D_addr)
{
	/* Search for address */
	int i = 0;
	for(i = 0; i < table->size; i++)
	{
		if(table->entry[i].D_addr == D_addr)
		{
			return i;
		}
	}

	/* Couldn't find address */
	return ERROR;
}

void UpdateEntryT(FTable *table, int D_addr, int valid, int NewOlink)
{
	/* Find index */
	int index = IndexT(table, D_addr);
	
	/* Error */
	if(index == ERROR)
	{
		return;
	}
	
	/* Give Entry */
	table->entry[index].OlinkID = NewOlink;
	table->entry[index].valid = valid;
}

void UpdateIndexT(FTable *table, int index, int valid, int NewOlink)
{
	/* Check for valid index */
	if(index > table->size)
	{
		return;
	}

	table->entry[index].OlinkID = NewOlink;
	table->entry[index].valid = valid;
}

int GetLink(FTable *table, int D_addr)
{
	/* Get index */
	int index = IndexT(table, D_addr);
	
	/* Error */
	if(index == ERROR)
	{
		return ERROR;
	}
	
	return table->entry[index].OlinkID;
}

/* Switch */
void switchMain(switchState *switchS)
{
	/* Variables */
	int i = 0;
	int j = 0;
	int packet_size = 0;
	int Olink = 0;
	int Ilink = 0;
	packetBuffer temp[5];
	int size = 0;
	
	/* Poll for packets */
	while(1)
	{
		for(i = 0; i < switchS->Isize; i++)
		{
			packet_size = linkReceive(&(switchS->link_I[i]),temp,
						&size);
			
			/* Packet Found */
			if(packet_size > 0)
			{
				for(j = 0; j < size; j++)
				{
					UpdateT(&(switchS->table),temp[j].valid,
						temp[j].srcaddr, i);
					
					push(&(switchS->Qpacket),temp[j]);
				}
			}
		}

		/* Queue has packets */
		if(switchS->Qpacket.size != 0)
		{
			/* Get first packet */
			temp[0] = pop(&(switchS->Qpacket));

			/* Valid Address */
			if(temp[0].srcaddr != -1)
			{
				/* Obtain link */
				Olink = GetLink(&(switchS->table),
						temp[0].dstaddr);

				/* Check the table */
				if(Olink != ERROR)
				{
					/* Send on link */
					linkSend(&(switchS->link_O[Olink]),
						&temp[0]);
				}
				else
				{
					Ilink = GetLink(&(switchS->table),
							temp[0].srcaddr);
					
					/* Send to all other links */
					for(i = 0; i < switchS->Osize;i++)
					{
						if(i != Ilink)
						{
							linkSend(&(switchS->link_O[i]), &(temp[0]));
						}
					}
				}
			}
		}
		
		/* Sleep */
		usleep(TENMILLISEC);
	}
}

void switchInit(switchState *switchS, int physID)
{
	switchS->Isize = 0;
	switchS->Osize = 0;
	switchS->physID = physID;
	initQ(&(switchS->Qpacket));
	InitTable(&(switchS->table));
}
