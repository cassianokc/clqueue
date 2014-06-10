#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "common.h"
#include "squeue.h"

void squeue(void);
void clqueue(void);

int main(int argc, char **argv)
{
	squeue();
	clqueue();
	return EXIT_SUCCESS;
}


void clqueue(void)
{
}


void squeue(void)
{
	int a, b;
	struct squeue *queue = squeue_init(10, sizeof(int));	
	srand(time(NULL));
	assert(queue!=NULL);
	a=1;
	squeue_insert(queue,&a); 	
	assert(!squeue_is_empty(queue));
	a=2;
	squeue_insert(queue,&a); 	
	assert(!squeue_is_full(queue));
	b=0;
	squeue_remove(queue, &b);
	assert(b==1);
	squeue_remove(queue, &b);
	assert(squeue_is_empty(queue));
	for (int i=0; i<1000; i++)
	{
		for (int j=0; j<1000; j++)
		{
			if (rand()%2==0)
				squeue_insert(queue, &j);
			else
				squeue_remove(queue, &j);
		}
		for (int j=0; j<10; j++)
			squeue_remove(queue, &a);
		for (int j=0; j<8; j++)
			squeue_insert(queue, &j);
		for (int j=0; j<8; j++)
		{
			squeue_remove(queue, &a);
			assert(a==j);
		}
	}
	squeue_free(queue);
}



