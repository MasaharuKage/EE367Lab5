/*
 * This is the source code of functions that will
 *
 *   - Set up and close links between nodes.  
 *   - Set up and close connections between the manager and hosts.
 *
 * In the program, connections are pipes, and links can be
 * pipes (and, in the future, sockets).
 *
 * Initially, all the connections and links are created.
 * But as the processes are forked, individual processes
 * will not need all the connections and links.  So they
 * close the unnecessary ones.
 *
 * The connections and link are nonblocking, which means
 * that they can be accessed but will not stop a process.
 * 
 * For example, if a process is reading a blocking pipe
 * that doesn't have enough bytes (e.g., it's empty) then
 * the process will wait until the pipe has sufficient
 * bytes.  So the process can get stuck.
 * 
 * On the other hand if the pipe is nonblocking then 
 * whenever a process reads a pipe that doesn't have enough
 * bytes then it continues processing and goes on to
 * do something else.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include "main.h"
#include "utilities.h"
#include "link.h"
#include "man.h"
#include "host.h"

#define EMPTY_ADDR  0xffff  /* Indicates that the empty address */
                             /* It also indicates that the broadcast address */
#define MAXBUFFER 1000
#define PIPEWRITE 1 
#define PIPEREAD  0

/* 
 * Create nonblocking connections(pipes) between manager and hosts  
 * assuming host physical IDs are 0, 1, ....
 */
void netCreateConnections(manLinkArrayType * manLinkArray) 
{
int i;
int pflag;

for (i=0; i<manLinkArray->numlinks; i++) {
   if (pipe(manLinkArray->link[i].toHost) < 0) {
      printf("Creating pipe failed\n");
      return;
   }
   if (pipe(manLinkArray->link[i].fromHost) < 0) {
      printf("Creating pipe failed\n");
      return;
   }

   /* Set the pipes to nonblocking */
   pflag = fcntl(manLinkArray->link[i].toHost[0],F_GETFL);
   fcntl(manLinkArray->link[i].toHost[0],F_SETFL, pflag | O_NONBLOCK);

   pflag = fcntl(manLinkArray->link[i].toHost[1],F_GETFL);
   fcntl(manLinkArray->link[i].toHost[1],F_SETFL, pflag | O_NONBLOCK);

   pflag = fcntl(manLinkArray->link[i].fromHost[0],F_GETFL);
   fcntl(manLinkArray->link[i].fromHost[0],F_SETFL, pflag | O_NONBLOCK);

   pflag = fcntl(manLinkArray->link[i].fromHost[1],F_GETFL);
   fcntl(manLinkArray->link[i].fromHost[1],F_SETFL, pflag | O_NONBLOCK);
}
}

/* 
 * Creates links to be used between nodes but does not set the
 * end nodes of the links
 */
void netCreateLinks(linkArrayType * linkArray)
{ 
int i;

for (i=0; i<linkArray->numlinks; i++) { 
   linkArray->link[i].linkID = i;
   linkArray->link[i].linkType = UNIPIPE;
   linkArray->link[i].uniPipeInfo.pipeType = NONBLOCKING;
   linkCreate(&(linkArray->link[i]));
}
}

/* 
 * Close all connections except the outgoing connection from
 * the host to manager and the incoming connection from
 * the manager to host.
 */
void netCloseConnections(manLinkArrayType *  manLinkArray, int hostid)
{
int i;

/* Close all connections not incident to the host */
for (i=0; i<manLinkArray->numlinks; i++) {
   if (i != hostid) { 
      close(manLinkArray->link[i].toHost[0]);
      close(manLinkArray->link[i].toHost[1]);
      close(manLinkArray->link[i].fromHost[0]);
      close(manLinkArray->link[i].fromHost[1]);
   }
}

/* Close manager's side of the connection from host to manager */
close(manLinkArray->link[hostid].fromHost[PIPEREAD]);

/* Close manager's side of the connection from manager to host */
close(manLinkArray->link[hostid].toHost[PIPEWRITE]);
}

/*
 * Sets the end nodes of the links.  In this case there is
 * just two links between two hosts
 */

void netSetNetworkTopology(linkArrayType * linkArray)
{
linkArray->link[0].uniPipeInfo.physIdSrc = 0;
linkArray->link[0].uniPipeInfo.physIdDst = 1;
linkArray->link[1].uniPipeInfo.physIdSrc = 1;
linkArray->link[1].uniPipeInfo.physIdDst = 0;
}

/*
 * Find host's outgoing link and return its index
 * from the link array
 */
int netHostOutLink(linkArrayType * linkArray, int hostid) 
{
int i;
int index;

index = linkArray->numlinks;

for (i=0; i<linkArray->numlinks; i++) {
   /* Store index if the outgoing link is found */
   if (linkArray->link[i].uniPipeInfo.physIdSrc == hostid) 
      index = i;
}
if (index == linkArray->numlinks) 
   printf("Error:  Can't find outgoing link for host\n");
return index; 
}

/*
 * Find host's incoming link and return its index
 * from the link array
 */
int netHostInLink(linkArrayType * linkArray, int hostid) 
{
int i;
int index;

index = linkArray->numlinks;

for (i=0; i<linkArray->numlinks; i++) {
   /* Store index if the outgoing link is found */
   if (linkArray->link[i].uniPipeInfo.physIdDst == hostid) index = i;
}
if (index == linkArray->numlinks) 
   printf("Error:  Can't find outgoing link for host\n");
return index; 
}

/*
 * Close links not connected to the host
 */
void netCloseHostOtherLinks(linkArrayType * linkArray, int hostid)
{
int i;

for (i=0; i<linkArray->numlinks; i++) {
   if (linkArray->link[i].uniPipeInfo.physIdSrc != hostid) 
      close(linkArray->link[i].uniPipeInfo.fd[PIPEWRITE]);
   if (linkArray->link[i].uniPipeInfo.physIdDst != hostid) 
      close(linkArray->link[i].uniPipeInfo.fd[PIPEREAD]);
}
}

/* Close all links*/
void netCloseLinks(linkArrayType * linkArray) 
{
int i;

for (i=0; i<linkArray->numlinks; i++) 
   linkClear(&(linkArray->link[i]));
}


/* Close the host's side of a connection between a host and manager */
void netCloseManConnections(manLinkArrayType * manLinkArray)
{
int i;

for (i=0; i<manLinkArray->numlinks; i++) {
   close(manLinkArray->link[i].toHost[PIPEREAD]);
   close(manLinkArray->link[i].fromHost[PIPEWRITE]);
}
}


