/* 
 * host.h 
 */
#ifndef HOST_H_
#define HOST_H_

#define NAME_LENGTH 100 

#include "databuff.h"

typedef struct { /* State of host */
   int   physid;              /* physical id */
   char  maindir[NAME_LENGTH]; /* main directory name */
   int   maindirvalid;        /* indicates if the main directory is empty */
   int   netaddr;             /* host's network address */
   int   nbraddr;             /* network address of neighbor */
   packetBuffer sendPacketBuff;  /* send packet buffer */
   packetBuffer rcvPacketBuff;   
   managerLink manLink;       /* Connection to the manager */
   LinkInfo linkin;           /* Incoming communication link */
   LinkInfo linkout;          /* Outgoing communication link */

   /* Added */
   int rcvflag;
   DataBuff sendbuff;
   DataBuff rcvbuff;
} hostState;

void hostMain(hostState * hstate);

void hostInit(hostState * hstate, int physid);

#endif
