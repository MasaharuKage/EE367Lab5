#ifndef NET_H_
#define NET_H_

#include "switch.h"

/* Create nonblocking connections from the manager to all hosts */
void netCreateConnections(manLinkArrayType * manLinkArray); 

/* Create all nonblocking links */
void netCreateLinks(linkArrayType * linkArray);

/* 
 * Close all connections except the outgoing connection
 * from the host to manager and the incoming connection from
 * the manager to host
 */ 
void netCloseConnections(manLinkArrayType *  manLinkArray, int hostid);

/* 
 * Set up the end nodes of the links -- essentially creating 
 * network topology
 */
void netSetNetworkTopology(linkArrayType * linkArray, int src[], int dst[]);

/* Find host's outgoing link and return its index from the link array */
int netHostOutLink(linkArrayType * linkArray, int hostid); 

/* Find host's incoming link and return its index from the link array */
int netHostInLink(linkArrayType * linkArray, int hostid); 

/* Close links not used by the host */
void netClearHostOtherLinks(linkArrayType * linkArray, int hostid);

/* Close all links */
void netCloseLinks(linkArrayType * linkArray); 

/* Close the host's side of a connection between a host and manager */
void netCloseManConnections(manLinkArrayType * manLinkArray);

/* Added */
void netCloseAllManLinks(manLinkArrayType *manLinkArray);
void netSwitchLinks(linkArrayType *linkArray,switchState *switchS,int switchID);
void netCloseSwitchOtherLinks(linkArrayType *linkArray, int hostid);

#endif
