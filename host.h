/* 
 * host.h 
 */

#define NAME_LENGTH 100 

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
} hostState;

void hostMain(hostState * hstate);

void hostInit(hostState * hstate, int physid);

