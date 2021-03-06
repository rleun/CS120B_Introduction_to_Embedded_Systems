/*	Partner 1 Name & E-mail: Richel Leung - rleun005@ucr.edu 
 *	Partner 2 Name & E-mail: Patrick Boyle - pboyl001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab #0  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include "rims.h"

typedef unsigned char uchar;

// high and low parameters are declared as POINTERS to uchars

void Nibbles(uchar num, uchar *high, uchar *low) {
 *high = num >> 4; // Note the "*" before high
 *low = num & 0x0F; // Note the "*" before low
 return;
}

uchar i, j;

void main(){
 uchar num;
 while(1) {
 num = A;
 Nibbles(num, &i, &j); // Passing i/j ADDRESSES
 puts("High nibble: "); puti(i);
 puts(" Low nibble: "); puti(j); puts("\r\n");
 while (num == A) {} // Print once; wait for change
 }
}