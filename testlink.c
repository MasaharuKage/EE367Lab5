#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "link.h"

main() 
{

packetBuffer pb1, pb2, pb3;
LinkInfo link;


link.linkType = UNIPIPE;
link.linkID = 0;
link.uniPipeInfo.pipeType=NONBLOCKING;
linkCreate(&link);

pb1.valid = 1;
pb1.length=10;
pb1.srcaddr=123;
pb1.dstaddr=456;
strcpy(pb1.payload, "Okay, here we go");

linkSend(&link, &pb1);
linkReceive(&link, &pb2);
linkReceive(&link, &pb3);

}



