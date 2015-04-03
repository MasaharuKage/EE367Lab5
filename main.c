#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <fcntl.h>

#include "main.h"
#include "utilities.h"
#include "link.h"
#include "man.h"
#include "host.h"
#include "net.h"

#define EMPTY_ADDR  0xffff  /* Indicates the empty address */
                             /* It also indicates that the broadcast address */
#define MAXBUFFER 1000
#define PIPEWRITE 1 
#define PIPEREAD  0

void main()
{
hostState hstate;             /* The host's state */
linkArrayType linkArray;
manLinkArrayType manLinkArray;

/* Added */
switchState switchS;
char file[1000];
char word[1000];
int src[20];
int dst[20];
int count = 0;
int numHosts = -1;
int numLinks = -1;
int numSwitch = -1;

pid_t pid;  /* Process id */
int physid; /* Physical ID of host */
int i;
int k;

/* Get file */
printf("Enter a file: ");
scanf("%s", file);

/* Read from the file */
FILE * f = fopen(file, "r");
if(file != NULL)
{
	/* Get information */
	char data[128];
	fgets(data, sizeof data, f);
	findWord(word, data, 1);
	numHosts = ascii2Int(word);
	findWord(word, data, 2);
	numLinks = ascii2Int(word);
	findWord(word, data, 3);
	numSwitch = ascii2Int(word);

	/* Obtain sources and destinations */
	while(fgets(data, sizeof data, f) != NULL)
	{
		findWord(word, data, 1);
		src[count] = ascii2Int(word);
		findWord(word, data, 2);
		dst[count] = ascii2Int(word);
		count++;
	}

	/* Close the file */
	fclose(f);
}
else
{
	perror(file);
	return;
}

/* Check for valid data */
if(numHosts == -1 || numLinks == -1 || numSwitch == -1 || count != numLinks/2
	|| numLinks%2 != 0)
{
	printf("Invalid Information\n");
	return;
}

/* Allocation for Dynamic array of links */
manLinkArray.link = (managerLink*)malloc(numHosts*sizeof(managerLink));
linkArray.link = (LinkInfo*)malloc(numLinks*sizeof(LinkInfo));
switchS.link_I = (LinkInfo*)malloc(numHosts*sizeof(LinkInfo));
switchS.link_O = (LinkInfo*)malloc(numHosts*sizeof(LinkInfo));

/* 
 * Create nonblocking (pipes) between manager and hosts 
 * assuming that hosts have physical IDs 0, 1, ... 
 */
manLinkArray.numlinks = numHosts;
netCreateConnections(& manLinkArray);

/* Create links between nodes but not setting their end nodes */

linkArray.numlinks = numLinks;
netCreateLinks(& linkArray);

/* Set the end nodes of the links */

netSetNetworkTopology(& linkArray, src, dst);

/* Create nodes and spawn their own processes, one process per node */ 

for (physid = 0; physid < numHosts; physid++) {

   pid = fork();

   if (pid == -1) {
      printf("Error:  the fork() failed\n");
      return;
   }
   else if (pid == 0) { /* The child process -- a host node */

      hostInit(&hstate, physid);              /* Initialize host's state */

      /* Initialize the connection to the manager */ 
      hstate.manLink = manLinkArray.link[physid];

      /* 
       * Close all connections not connect to the host
       * Also close the manager's side of connections to host
       */
      netCloseConnections(& manLinkArray, physid);

      /* Initialize the host's incident communication links */

      k = netHostOutLink(&linkArray, physid); /* Host's outgoing link */
      hstate.linkout = linkArray.link[k];

      k = netHostInLink(&linkArray, physid); /* Host's incoming link */
      hstate.linkin = linkArray.link[k];

      /* Close all other links -- not connected to the host */
      netCloseHostOtherLinks(& linkArray, physid);

      /* Go to the main loop of the host node */
      hostMain(&hstate);
      
      /* Free memory */
      free(manLinkArray.link);
      free(linkArray.link);
      free(switchS.link_I);
      free(switchS.link_O);
      
   }  
}

/* Create the switches */
for(physid = numHosts; physid < numHosts + numSwitch; physid++)
{
	pid = fork();

	if (pid == -1) 
	{
      		printf("Error:  the fork() failed\n");
      		return;
   	}
	else if(pid == 0)
	{
		/* Create a switch */
		switchInit(&switchS, physid);
		
		/* Close all links */
		netCloseAllManLinks(&manLinkArray);

		/* Generate Switch Links */
		netSwitchLinks(&linkArray, &switchS, physid);

		/* Close all other connections */
		netCloseSwitchOtherLinks(&linkArray, physid);

		/* Run the switch */
		switchMain(&switchS);
		
		/* Free Memory */
		free(switchS.link_I);
		free(switchS.link_O);
		free(linkArray.link);
		free(manLinkArray.link);
	}
}

/* Manager */

/* 
 * The manager is connected to the hosts and doesn't
 * need the links between nodes
 */

/* Close all links between nodes */
netCloseLinks(&linkArray);

/* Close the host's side of connections between a host and manager */
netCloseManConnections(&manLinkArray);

/* Go to main loop for the manager */
manMain(& manLinkArray);

/* Free memory */
free(switchS.link_O);
free(switchS.link_I);
free(linkArray.link);
free(manLinkArray.link);

/* 
 * We reach here if the user types the "q" (quit) command.
 * Now if we don't do anything, the child processes will continue even
 * after we terminate the parent process.  That's because these
 * child proceses are running an infinite loop and do not exit 
 * properly.  Since they have no parent, and no way of controlling
 * them, they are called "zombie" processes.  Actually, to get rid
 * of them you would list your processes using the LINUX command
 * "ps -x".  Then kill them one by one using the "kill" command.  
 * To use the kill the command just type "kill" and the process ID (PID).
 *
 * The following system call will kill all the children processes, so
 * that saves us some manual labor
 */
kill(0, SIGKILL); /* Kill all processes */
}
