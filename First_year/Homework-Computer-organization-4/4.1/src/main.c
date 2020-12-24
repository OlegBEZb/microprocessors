#include <stdint.h>
#include <stdbool.h> 
#include <stdlib.h>

void sort(int32_t * buf, uint32_t size);
int32_t getMedian(int32_t * buf, uint32_t size);

int main(void)
{
	int32_t mas[]={-1,1,0,4,5,0x51,14444,-5,10,9};
	sort(mas,(uint32_t)(sizeof(mas)/4));
	
	int32_t buf[]={1,3,2,6,-432,0x2222,11,4,5,7,6,8,9};
	volatile int32_t answer=getMedian(buf,(uint32_t)(sizeof(buf)/4));

	while(1);
	return 0;
}

/*gnome sort from smallest to biggest. If you are at the beginning of the array 
or if the current element is more or equal to the previous, go forward. 
Othewise - swap them and go back. If you are going forward and there is no elements, 
array is sorted.
*/ 
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
 
/* For every element counts how many elements are more and how many are less than
current. Variable "sum" is a difference between number of more cases and less cases.
Variable "equal" is number of elements that are equal to the current.
The problem is that we don't know how to classify elements that are equal for the current:
they can be "less or equal" or "more or equal". I explain my algorythm this way:
we have "more than" cases (+ points), "less than" cases (- points) and "equal" cases (=points).
And we can reduce the difference between + and - points with = points. Our goal
is to make this difference equal to zero. If there is few = points to "seal a hole"
the current element is not a median. But if we can "seal a hole" with the number
of = points, we can reach 2 cases: we have odd or even number of =points left.
If the number is even, we can divide equally them between + and - points, so we will
have an equal amount of numbers that are "less than" and "more than" the current. So current
number is median. And if the number is odd, we can't divide = points equally,so
the current number will not be a median.   
*/
int32_t getMedian(int32_t * buf, uint32_t size)
{
	int sum=0;
	int equal=0;
	for(int i=0;i<size;i++)
	{
		for(int j=0;j<size;j++)
		{
			if(i==j) continue;
			if(buf[i]>=buf[j])
			{
				if(buf[i]>buf[j]) sum++;
				else equal++;
			}
			else sum--;
		}
		if(equal>=abs(sum))
		{
			if((equal-abs(sum))%2==0) return buf[i];
		}	
		equal=0;
		sum=0;
	}
}

