#include "CMSIS/stm32f10x.h"
#include <stdint.h>
#include <stdbool.h> 
#include <math.h>
#include <string.h>

uint8_t countSetBits(uint32_t n);

uint8_t countLeadingZeros( uint32_t n);
 
int main(void)
{
	volatile uint32_t example=0x1FFFFFFF;
	volatile uint8_t result=countLeadingZeros(example);
	
	while(1);
	return 0;
}

	uint8_t countSetBits(uint32_t n)
{
	return ((n&1)==1)+((n&2)==2)+((n&4)==4)+((n&8)==8)+((n&16)==16)+((n&32)==32)
	+((n&1<<6)==(1<<6))+((n&1<<7)==(1<<7))+((n&1<<8)==(1<<8))+((n&1<<9)==(1<<9))
	+((n&1<<10)==(1<<10))+((n&1<<11)==(1<<11))+((n&1<<12)==(1<<12))
	+((n&1<<13)==(1<<13))+((n&1<<14)==(1<<14))+((n&1<<15)==(1<<15))
	+((n&1<<16)==(1<<16))+((n&1<<17)==(1<<17))+((n&1<<18)==(1<<18))
	+((n&1<<19)==(1<<19))+((n&1<<20)==(1<<20))+((n&1<<21)==(1<<21))
	+((n&1<<22)==(1<<22))+((n&1<<23)==(1<<23))+((n&1<<24)==(1<<24))
	+((n&1<<25)==(1<<25))+((n&1<<26)==(1<<26))+((n&1<<27)==(1<<27))
	+((n&1<<28)==(1<<28))+((n&1<<29)==(1<<29))+((n&1<<30)==(1<<30))
	+((n&1u<<31)==(1u<<31));
}
	uint8_t countLeadingZeros( uint32_t n)
{
	volatile uint32_t numerals=0xffffffff; //set a number consisting only of ones 
	volatile uint32_t number=(numerals-((n&1)==1))&(numerals-((n&1<<1)==(1<<1))*(numerals>>30))&
	(numerals-((n&1<<2)==(1<<2))*(numerals>>29))&(numerals-((n&1<<3)==(1<<3))*(numerals>>28))&
	(numerals-((n&1<<4)==(1<<4))*(numerals>>27))&(numerals-((n&1<<5)==(1<<5))*(numerals>>26))&
	(numerals-((n&1<<6)==(1<<6))*(numerals>>25))&(numerals-((n&1<<7)==(1<<7))*(numerals>>24))&
	(numerals-((n&1<<8)==(1<<8))*(numerals>>23))&(numerals-((n&1<<9)==(1<<9))*(numerals>>22))&
	(numerals-((n&1<<10)==(1<<10))*(numerals>>21))&(numerals-((n&1<<11)==(1<<11))*(numerals>>20))&
	(numerals-((n&1<<12)==(1<<12))*(numerals>>19))&(numerals-((n&1<<13)==(1<<13))*(numerals>>18))&
	(numerals-((n&1<<14)==(1<<14))*(numerals>>17))&(numerals-((n&1<<15)==(1<<15))*(numerals>>16))&
	(numerals-((n&1<<16)==(1<<16))*(numerals>>15))&(numerals-((n&1<<17)==(1<<17))*(numerals>>14))&
	(numerals-((n&1<<18)==(1<<18))*(numerals>>13))&(numerals-((n&1<<19)==(1<<19))*(numerals>>12))&
	(numerals-((n&1<<20)==(1<<20))*(numerals>>11))&(numerals-((n&1<<21)==(1<<21))*(numerals>>10))&
	(numerals-((n&1<<22)==(1<<22))*(numerals>>9))&(numerals-((n&1<<23)==(1<<23))*(numerals>>8))&
	(numerals-((n&1<<24)==(1<<24))*(numerals>>7))&(numerals-((n&1<<25)==(1<<25))*(numerals>>6))&
	(numerals-((n&1<<26)==(1<<26))*(numerals>>5))&(numerals-((n&1<<27)==(1<<27))*(numerals>>4))&
	(numerals-((n&1<<28)==(1<<28))*(numerals>>3))&(numerals-((n&1<<29)==(1<<29))*(numerals>>2))&
	(numerals-((n&1<<30)==(1<<30))*(numerals>>1))&(numerals-((n&1u<<31)==(1u<<31))*(numerals));
	return countSetBits(number);
}
