#include "CMSIS/stm32f10x.h"

struct massiv
{
	int32_t mas[11];
};

struct massiv sort(struct massiv a);

int main(void)
{
	struct massiv sample={{2,1,3,5,5,4,7,8,11,10,9}};
	
	sample=sort(sample);
	
	while(1);
		
	return 0;
}

struct massiv sort(struct massiv a)
{
	int i=0;
	int temp;
	while (i< 11)
	{
		if((i==0)||(a.mas[i-1]<=a.mas[i])) i++;
		else
		{
			temp=a.mas[i];
			a.mas[i]=a.mas[i-1];
			a.mas[i-1]=temp;
			i--;
		}
	}
	return a;
}
