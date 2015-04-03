/* Definitions and prototypes for the link (link.c)
 */

#ifndef LINK_H_
#define LINK_H_

#include "main.h"
#include "databuff.h"

//#define NUMLINKS 2

enum LinkType {UNIPIPE}; /* UNIPIPE = unidirectional pipe
                          * We can add more types later
                          */

enum UniPipeType {BLOCKING, NONBLOCKING};

typedef struct {
   enum UniPipeType pipeType;
   int         fd[2];
   int         physIdSrc;
   int	       physIdDst;
} UniPipeInfo;

typedef struct {  /* Has all the information to implement a link */
   int linkID;             /* ID for the link */
   enum LinkType linkType; /* The type of link */
   UniPipeInfo uniPipeInfo; /* If the link is a pipe, this is the information */
} LinkInfo;


typedef struct {
   int numlinks;
   LinkInfo *link;
} linkArrayType;

/* Transmit the packet in pbuff on the link */
int linkSend(LinkInfo * link, packetBuffer * pbuff);

/* Downloads a packet from the link into pbuff */
int linkReceive(LinkInfo * link, packetBuffer pbuff[], int *index);

/* Closes a link */
int linkClear(LinkInfo * link);

/* Initializes a link */
int linkCreate(LinkInfo * link);

#endif
