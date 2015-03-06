/* A collection of useful functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

#define MAXBUFFER 1000 /* Maximum size of char buffer */


/*
 * Pre-condition: a character strings c1[] and c2[]
 * Post-condition: appended to c1[] is a single space and then
 *                 c2[] is appended 
 */
void appendWithSpace(char c1[], char c2[])
{
strcat(c1," ");
strcat(c1,c2);
}



/* 
 * Pre-condition:  An ascii digit in input c
 * Post-condition:  Return the value of the digit or
 *                  -1 if it's not a digit
 */

int asciiValue(char c)
{
int value;

if (c < '0' || c > '9') return -1;
value = c - '0';
return value;
}


/* 
 * Pre-condition: input integer value "value"
 * Post-condition: character string c[] is a text representation of the value
 */

void int2Ascii(char c[], int value)
{
char reverse[MAXBUFFER]; /* String to store digits in the reverse order */
int i, j, k;
int remainder; 

if (value < 0) {
   c[0] = '\0';  /* return empty character string */
   printf("Error:  negative value passed to int2Ascii\n");
   return;
}

k = 0;
remainder = value;

do { /* Compute last digit to the first */
   reverse[k] = (remainder % 10) + '0';
   remainder = remainder/10;
   k++;
} while(remainder > 0);

/* copy the reverse of reverse[] into c[] */
for (i=0; i<k; i++) c[i] = reverse[k-i-1];
c[k] = '\0'; /* terminate c[] */
}

/* 
 * Pre-condition:  Character string c[] is an unsigned number.
 * Post-condition: Return the value if it is a valid number
 *                 Return -1, if the input is invalid
 */

int ascii2Int(char c[]) 
{
int first, last; /* position of first and last digits */
int value;       /* value of number */

/* Have first point to the first digit if it exists */
first = 0;
while (asciiValue(c[first]) == -1 && c[first] != '\0') first++;
if (c[first] == '\0') return -1;

/* Have last point to the last digit */
last = first;
while (asciiValue(c[last]) != -1 && c[last] != '\0') last++;
last--; 

/* Compute the value */
value = 0;
for(value = 0; first <= last; first++) 
   value = 10* value + asciiValue(c[first]);

return value;
}

/* 
 * Pre-condition:  c[] is a character string of words.  Words are assumed
 *                 to be numbered 1, 2, ...   The value k is
 *                 an integer > 0.
 * Post-condition:  Returns the index of where the kth word begins.
 */

int point2Word(char c[], int k)
{
int i;
int count; /* counts number of words found */

count = 0;
if (k<1) return -1; /* character string is empty */

for (i=0; c[i]==' '; i++); /* Skip over spaces */

if (c[i] == '\0') return -1; /* String only has spaces */
else count=1; /* i is pointing to a word */

while (count < k) {

   /* Find the next word */
   for (; c[i] != ' ' && c[i] != '\0'; i++); /* skip over word */
   if (c[i] == '\0') return -1; /* Not enough words in the string */

   for (; c[i]==' '; i++); /* Skip over spaces */
   if (c[i] == '\0') return -1; /* Didn't find another word */
   else count++;           /* We reached another word, so increment count */

} 

return i;
}

/*
 * Pre-condition:  c[] has words, numbered 1, 2, ...  The value k is
 *                 a positive integer greater than 0.
 * Post-condition: word[] has the kth word.  Its empty if there are no
 *                 such word.
 */

void copyWord(char word[], char c[], int k)
{
int i, j;

i=0;
for(j=k; c[j]!= '\0' && c[j]!=' '; j++) {
   word[i] = c[j];
   i++;
}  
word[i] = '\0';
}


/*
 * Input:  character string c[].  It assumes that the string has
 *         words separated by spaces.  The words are numbered
 *         1, 2, ...   The input k denotes the kth word to search for. 
 * Output:  the kth word, which is stored in word[]
 */

void findWord(char word[], char c[], int k)
{
int i;

i = point2Word(c, k);
if (i < 0) {
   word[0] = '\0';
   return;
}
copyWord(word, c, i);
}

