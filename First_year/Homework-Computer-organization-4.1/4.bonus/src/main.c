#include "CMSIS/stm32f10x.h"
#include "stdlib.h"

// здесь 0 <= j < size - номер нужной позиции, начиная с нуля
int32_t getOrderStatistic(int32_t * buf, uint32_t size, uint32_t j);

void sort(int32_t * buf, uint32_t size);

int main(void)
{
	int32_t const N=10;//задаём размер массива
	int32_t buf[N];
	
	volatile int32_t correct=0;
	for(int counter=0; counter<100;counter++)
	{	
	
	for(int i=0; i<N; i++) 
	{
		buf[i] = rand()%100;
	}//массив инициализируется случайными числами от -500 до 499 
	
	volatile int32_t j=rand()%N;//жипорядковая статистика

	volatile int32_t answer=getOrderStatistic(buf,N,j);//что даёт честно украденый алгоритм
	
	sort(buf,N);
	volatile int32_t check=buf[j];
	correct+=(answer==check);//проверяем
	
	}
	
	while(1);
	return 0;
}

int32_t getOrderStatistic(int32_t * buf, uint32_t size, uint32_t j)
{
	int32_t left = 0, right =size-1, temp;
  while (1) 
	{		
     int32_t v = buf[(left + right) / 2];
     int l = left;
     int r = right;
     while (l<=r) 
		 {
        while (buf[l] < v)l++;
        while (buf[r] > v)r--;
        if (l<=r) 
        {
					temp=buf[l];
					buf[l]=buf[r];
					buf[r]=temp;
					l++;
					r++;
				}
			}
			int32_t mid=r;

    if (mid == j) {
      return buf[mid];
    }
    else if (j < mid) {
      right = mid;
    }
    else {
      j -= mid + 1;
      left = mid + 1;
    }
	}

	
}


void sort(int32_t * buf, uint32_t size)
{
	int i=0;
	int temp;
	while (i< size)
	{
		if((i==0)||(buf[i-1]<=buf[i])) i++;
		else
		{
			temp=buf[i];
			buf[i]=buf[i-1];
			buf[i-1]=temp;
			i--;
		}
	}
}
