#ifndef MAIN_H_
#define MAIN_H_

#define PAYLOAD_LENGTH 200 /* Maximum payload size */

typedef struct { /* Packet buffer */
   int srcaddr;  /* Source address */
   int dstaddr;  /* Destination addres */
   int length;   /* Length of packet */
   char payload[PAYLOAD_LENGTH + 1];  /* Payload section */
   int valid;   /* Indicates if the contents is valid */ 

   /* Added */
   int start;
   int end;
} packetBuffer;

   
#endif
