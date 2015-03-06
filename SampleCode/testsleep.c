#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TENMILLISEC 10000  

main()
{
int i, k;

for (i=0; i<10; i++) {
   for (k=0; k<50; k++) {
   usleep(TENMILLISEC);
   }
   printf("%d\n",i);
}


}



