/* File: switch.h
 * Name: Ryan Walser
 * Group: Michele Shimoda, Scott Nakashima
 */

#ifndef SWITCH_H_
#define SWITCH_H_

/* Libraries */
#include <stdio.h>
#include <sys/types.h>
#include "main.h"

/* Macros */
#define TRUE 1
#define FALSE 0
#define ERROR -1
#define ROW 10
#define COLUMN 2
#define TENMILLISEC 10000
#define PIPEWRITE 1
#define PIPEREAD 0

/* Queue */
typedef struct{
	packetBuffer elements[10000];
	int head;
	int tail;
	int size;
} Queue;

/* Forwarding Table */
typedef struct{
	int OlinkID;
	int D_addr;
	int valid;
} FTEntry;
typedef struct{
	FTEntry entry[100];
	int size;
} FTable;

/* Switch struct */
typedef struct{
	int Isize;
	int Osize;
	int physID;
	LinkInfo *link_I;
	LinkInfo *link_O;	
	FTable table;
	Queue Qpacket;
} switchState;

/* Queue Functions */
void initQ(Queue *q);
void push(Queue *q, packetBuffer data);
packetBuffer pop(Queue *q);

/* Table Functions */
void InitTable(FTable *table);
void UpdateT(FTable *table, int valid, int D_addr, int OlinkID);
int Olink(FTable *table, int D_addr);
void AddT(FTable *table, int valid, int D_addr, int OlinkID);
int IndexT(FTable *table, int D_addr);
void UpdateEntryT(FTable *table, int D_addr, int valid, int NewOlink);
void UpdateIndexT(FTable *table, int index, int valid, int NewOlink);
int GetLink(FTable *table, int D_addr);

/* Switch Functions */
void switchMain(switchState *switchS);
void switchInit(switchState *switchS, int physID);

#endif
