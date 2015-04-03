/* File: databuff.h
 * Name: Ryan Walser
 * Group: Michele Shimoda, Scott Nakashima
 */
#ifndef DATABUFF_H_
#define DATABUFF_H_

#define MAX_DATA_LENGTH 2000

typedef struct{
	char data[MAX_DATA_LENGTH];
	int length;
	int busy;
	int pos;
	int dstaddr;
	int valid;
	int srcaddr;
	int start;
	int end;
} DataBuff;

#endif
