/*
 * Source code for the manager.  
 *
 * The main loop of the manager is manMain().  Repeatedly
 * gets a command from the user and then executes the command.
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

#define NAME_LENGTH 100
#define EMPTY_ADDR  10000  /* Indicates that the empty address */
                             /* It also indicates that the broadcast address */
#define MAXBUFFER 1000
#define PIPEWRITE 1 
#define PIPEREAD  0
#define TENMILLISEC 10000

/* Get a command from the user */
char manGetUserCommand(int chost);  

/*
 * Functions to fulfill user's commands 
 *
 *   - manSetNetAddr:  Set host's network address
 *   - manSetMainDir:  Set host's main directory
 *   - manClearRcvFlg:  Clear host's receive flag
 *   - manUploadPacket: Upload a file into the host's send packet buffer
 *   - manDownloadPacket:  Download the payload of the receive 
 *                           packet buffer into a file
 *   - manTransmitPacket:  Transmit a packet in the host's send packet buffer
 *   - manGetHostState:  Get the host's state
 *   - manDisplayHostState:    After getting the state, the manager displays
 *                              it on the console
 *   - manDisplayHosts: Lists all the hosts on the console, highlighting 
 *                          the currently connected host
 *   - manChangeHost: Change the host 
 */
void manSetNetAddr(managerLink * manLink);
void manSetMainDir(managerLink * manLink);
void manClearRcvFlg(managerLink * manLink);
void manUploadPacket(managerLink * manLink); 
void manDownloadPacket(managerLink * manLink); 
int  manTransmitPacket(managerLink * manLink);
void manGetHostState(managerLink * manLink);
void manDisplayHostState(char buffer[]);
void manDisplayHosts(int currhost, int maxHosts); 
int manChangeHost(int maxHosts);

/* 
 * Many of the commands by the user requires the manager to
 * send requests to the host.  The following functions will
 * support the interaction between the manager and host
 */

/* Sends a command (a character string) to the host.  */
void manCommandSend(managerLink * manLink, char command[]);

/* 
 * After sending a command to the host the manager waits for a reply
 * The function manWaitForReply waits for the reply.  It calls
 *    - manReplyReceive, which reads the pipe from the host and stores
 *          the result in a buffer
 *    - manDisplayReplyMsg, which displays the message sent by the host.
 */
void manWaitForReply(managerLink * manLink, int cmd);
int  manReplyReceive(managerLink * manLink, char reply[]);
void manDisplayReplyMsg(char replymsg[]);

/*
 * Functions
 */

/* 
 * Downloads the contents of the pipe to reply[].  Returns
 * The length of the download +1
 */
int manReplyReceive(managerLink * manLink, char reply[])
{
int n;

n = read(manLink->fromHost[PIPEREAD],reply,250);
reply[n] = '\0';
return n+1;
}

/*
 * Sends command[] to the host
 */
void manCommandSend(managerLink * manLink, char command[])
{
int i;
i = write(manLink->toHost[PIPEWRITE],command,strlen(command));
}

/*
 * This is called after the manager sends a command to the host.
 * It will wait for a reply from the host.  Since the connection (pipe)
 * is nonblocking, it keeps polling the pipe until something is
 * received (via the function manReplyReceive).  Currently, the
 * message from the host can have two forms:
 *
 * - The first word of the message is "DISPLAY".  This means
 *   that the rest of the message is to be displayed on the user's
 *   console.  For example, a message "DISPALY  Packet transmitted"
 *   will display "Packet transmitted" on the user's console
 *
 * - The first word of the message is "GetHostStateAck", which
 *   means that the rest of the message is the state of the host.
 *   This includes the host's main directory, network address, etc.
 *   These values are displayed on the user's console.
 *
 * Note that there is a 10 millisecond delay between pollings which
 * is implemented by the usleep(k) system call.  This will put the
 * process to sleep for k microseconds.  We allow this delay because
 * otherwise, the manager will loop very rapidly even though most
 * of the time the user doesn't enter a command.  So we may save
 * some processing cycles when the manager is asleep.  Also, it
 * gives some delay between the time when the manager sends a command to
 * the host and the time when a reply comes back from the host.
 */

void manWaitForReply(managerLink * manLink, int cmd)
{ 
char reply[1000];
char word[1000];
int length;

do {
   usleep(TENMILLISEC); /* Go to sleep for 10 milliseconds */
   length = manReplyReceive(manLink, reply);
   findWord(word, reply, 1);
   if (strcmp(word, "DISPLAY")==0) manDisplayReplyMsg(reply);
   else if (strcmp(word, "GetHostStateAck") == 0) manDisplayHostState(reply);
} while(length <= 1);
}

/* This displays the message after the first word on the user's console */
void manDisplayReplyMsg(char replymsg[])
{
int i;

/* prints from the second word onwards */
i = point2Word(replymsg, 2);  
for(; replymsg[i] != '\0'; i++) putchar(replymsg[i]); 
putchar('\n');
putchar('\n');
}

/* 
 * Gets a command from the user 
 */
char manGetUserCommand(int chost)  /* Gets command from the user */
{
char cmd;

while(1) {
   /* Display command options */
   printf("Commands (Current host ID = %d):\n", chost);
   printf("   (d) Display host's state\n");
   printf("   (s) Set host's network address\n");
   printf("   (m) Set host's main directory\n");
   printf("   (f) Clear host's receive packet buffer\n");
   printf("   (r) Download host's receive packet buffer into a file\n");
   printf("   (u) Upload file into host's send packet buffer\n");
   printf("   (t) Transmit packet from the host's send packet buffer\n");
   printf("   (h) Display all hosts\n");
   printf("   (c) Change host\n");
   printf("   (q) Quit\n");
   printf("   Enter Command: ");
   do {
      cmd = getchar();
   } while(cmd == ' ' || cmd == '\n'); /* get rid of junk from stdin */

   /* Ensure that the command is valid */
   if (cmd == 'd') return cmd;
   else if (cmd == 's') return cmd;
   else if (cmd == 'm') return cmd;
   else if (cmd == 'f') return cmd;
   else if (cmd == 'r') return cmd;
   else if (cmd == 'u') return cmd;
   else if (cmd == 't') return cmd;
   else if (cmd == 'q') return cmd;
   else if (cmd == 'h') return cmd;
   else if (cmd == 'c') return cmd;
   else printf("Invalid command, you entered %c\n", cmd);
   printf("\n");
}
}

/*
 * It tells the host to upload a file.  It asks the user to type in
 * the file name.  Then it sends the following message to
 * the host:  "UploadPacket <filename>"  
 */
void manUploadPacket(managerLink * manLink)
{
char fname[MAXBUFFER]; /* File name. */
char command[1000]; 

printf("Enter the name of the file in the host's directory: ");
scanf("%s",fname);

/* Create a command to the host */
command[0] = '\0'; /* Clear the command stringn */
appendWithSpace(command, "UploadPacket");
appendWithSpace(command, fname);

/* Send command to host */
manCommandSend(manLink, command);
} 

/* 
 * It tells the host to transmit a packet from its send packet buffer.
 * It first asks the user to enter the destination address of the packet.
 * Then it creates the message "Transmit <dest addr>".  Finally, it
 * sends the message.  It returns -1 on failure and 0 on success.
 */

int manTransmitPacket(managerLink * manLink)
{
char command[1000];
char word[1000];
int netaddr;

/* As the user to enter the destination address */
printf("Enter the destination's network address: ");
scanf("%d", &netaddr);
if (netaddr < 0) {
   printf("Aborted: address should be positive\n");
   return -1;
}
else if (netaddr > 10000) {
   printf("The address too big.  Try again.\n");
   return -1;
}

/* Create the command message */
command[0] = '\0';                         /* Empty command string */
appendWithSpace(command, "TransmitPacket"); 

int2Ascii(word, netaddr);    /* Add destination address */
appendWithSpace(command, word);  

/* Send command */
manCommandSend(manLink, command);  
return 0;
}

/*
 * It tells the host to download the payload of its receive packet
 * buffer into a file.  First, it asks the user to enter a file name.
 * Second, it creates a command message "DownloadPacket <file name>".
 * Then it sends the command message to the host.
 */

void manDownloadPacket(managerLink * manLink)
{
char fname[MAXBUFFER]; /* File name. */
char command[1000];

/* Enter file name */
printf("Enter the file name: ");
scanf("%s",fname);

/* Create a command to the host */
command[0] = '\0'; /* Clear the command */
appendWithSpace(command, "DownloadPacket");
appendWithSpace(command, fname);

/* Send command to the host. */
manCommandSend(manLink, command);
}


/* 
 * It tells the host to clear it's receive flag.
 * It sends the message "ClearRcvFlg" to the host. 
 */

void manClearRcvFlg(managerLink * manLink)
{
char command[100];

command[0] = '\0';    /* Empty the command string */
appendWithSpace(command, "ClearRcvFlg"); /* Create command */
manCommandSend(manLink, command); /* Send command to the host */
}

/*
 * It tells the host to set its main directory.  First, it
 * asks the user to enter the directory name.  Then it creates
 * the command message "SetMainDir <directory name>".  Then it
 * sends the message to the host.
 */
void manSetMainDir(managerLink * manLink)
{
char dirname[1000];
char command[1000];

/* Get the directory name */
printf("Enter new directory: ");
scanf("%s",dirname);

/* Create the command message */
command[0] = '\0'; /* Initialize command to the empty string */
appendWithSpace(command, "SetMainDir");
appendWithSpace(command, dirname);

/* Send the command message */
manCommandSend(manLink, command);
}


/* 
 * It tells the host to set its network address.  First, it
 * asks the user to enter the network address. Then it creates
 * the command message "SetNetAddr <network address>".  Then
 * it sends the message to the host.
 */

void manSetNetAddr(managerLink * manLink)
{
int netaddr;
char command[1000];
char addr[1000]; 

/* Ask user for the network address */
while(1) {
   printf("Enter network address: ");
   scanf("%d",&netaddr);
   if (netaddr <0)
      printf("Address must be postive. Try again\n");
   else if (netaddr > 10000)
      printf("The address is too big. Try again\n");
   else break;
}

/* Create the command message */
command[0] = '\0';
appendWithSpace(command, "SetNetAddr");
int2Ascii(addr,netaddr);
appendWithSpace(command, addr);

/* Send the message to the host */
manCommandSend(manLink, command);
}

/* 
 *  Tell the host to send its state information.  First, it
 *  creates the command message "GetHostState".  Then it
 *  sends the message to the host.
 */
void manGetHostState(managerLink * manLink)
{
char command[1000];

command[0] = '\0';                         /* Empty command string */
appendWithSpace(command, "GetHostState");  /* Create the command */
manCommandSend(manLink, command);          /* Send the command */
}

/* 
 * After the manager sends a request to get the host's
 * state information, the host will respond with a message
 * which has as its first word "GetHostStateAck", followed by
 *
 * - host's physical ID
 * - host's main directory
 * - host's network address
 * - host's neighbor address
 * - host's packet receive flag
 *
 * It displays these on the user's console
 */
void manDisplayHostState(char buffer[])
{

char word[1000];

findWord(word, buffer, 2);
printf("Host physical ID: %s\n", word);
findWord(word, buffer, 3);
printf("Directory: %s\n", word);
findWord(word, buffer, 4);
printf("Network address: %s\n", word);
findWord(word, buffer, 5);
printf("Neighbor address: %s\n", word);
findWord(word, buffer, 6);
printf("New packet received flag: %s\n", word);
printf("\n");
}

/*
 * Displays all the hosts on the user's console.  It
 * assumes that the hosts are numbered 0, 1,..., maxHosts-1.
 * It also assumes that "currhost" is the current host.
 * It highlights the current host.
 */
void manDisplayHosts(int currhost, int maxHosts) 
{
int i; 

printf("List of hosts:\n");

for (i=0; i<maxHosts; i++) {
   if (i == currhost) printf("   Host ID = %d (connected)\n",i);
   else printf("   Host ID = %d\n",i);
}
printf("\n");
}

/*
 * Returns a new host.  It assumes that the host numbers are 
 * 0, 1,..., maxHosts-1.  It asks the user to enter one of them.
 * It returns this number, which presumably will be the new host
 * that the manager is connected to.
 */ 
int manChangeHost(int maxHosts) 
{
int newnumber;

do {
   printf("Enter host ID number (range 0 to %d): ", maxHosts-1);
   scanf("%d",&newnumber);
   if (newnumber >= 0 && newnumber < maxHosts) break;
   else printf("Number is out of range, try again\n\n");
} while(1);

return newnumber;
}


/***************************** 
 * Main loop of the manager  *
 *****************************/
void manMain(manLinkArrayType * manLinkArray)
{
int currhost;      /* The current host the manager is connected to */
char cmd;          /* Command entered by user */
int k;

currhost = 0;      /* Manager is initially connected to host 0 */

while(1) {
   /* Get a command from the user */
   cmd = manGetUserCommand(currhost);

   /* Execute the command */
   if (cmd == 'q') return;
   else if (cmd == 'd') {
      manGetHostState(&(manLinkArray->link[currhost]));
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   } 
   else if (cmd == 's') {
      manSetNetAddr(&(manLinkArray->link[currhost]));
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   } 
   else if (cmd == 'm') { 
      manSetMainDir(&(manLinkArray->link[currhost]));
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   }
   else if (cmd == 'f') {
      manClearRcvFlg(&(manLinkArray->link[currhost]));
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   }
   else if (cmd == 'r') {
      manDownloadPacket(&(manLinkArray->link[currhost])); 
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   }
   else if (cmd == 'u') {
      manUploadPacket(&(manLinkArray->link[currhost])); 
      manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   }
   else if (cmd == 't') {
      k = manTransmitPacket(&(manLinkArray->link[currhost]));
      if (k==0) manWaitForReply(&(manLinkArray->link[currhost]), cmd);
   }
   else if (cmd == 'h') 
      manDisplayHosts(currhost, manLinkArray->numlinks);

   else if (cmd == 'c') 
      currhost = manChangeHost(manLinkArray->numlinks);

   else printf("***Invalid command, you entered %c\n", cmd);
}
printf("\n");
  
} 


