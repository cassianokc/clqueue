#include "cltask.cl"



#define SWAP(X, Y)  \
{					\
	int tmp = (X);  \
	X = (Y);		\
	Y = tmp;		\
}



__kernel void run(global struct pool *pool, global int *array)
{
	global int *task;
	int current_offset, array_offset, left, right, type;
	// Retrieves a new task from the task pool
	while (cltask_retrieve_task(pool, &task, &current_offset) != 0)
	{
		left = task[0];
		right = task[1];
		type = task[2];
		if (type == 0)
		{
			//If the sub-array is large enough, subdivide it.
			if (right-left > 300)
			{
				int new_task[3];
				int pivot, i, j;
				pivot = array[left];
				i = left; j = right+1;
				//Partition the array
				while(1)
				{
					do ++i; while( array[i] <= pivot && i <= right );
					do --j; while( array[j] > pivot );
					if( i >= j ) break;
					SWAP(array[i], array[j]);
				}
				SWAP(array[left], array[j]);
				cltask_set_dependant(pool, current_offset, 2);
				new_task[0] = left;
				new_task[1] = j-1;
				new_task[2] = 0;
				//Insert first partition in the task pool
				cltask_new_task(pool, new_task, current_offset);
				new_task[0] = j+1;
				new_task[1] = right;
				new_task[2] = 0;
				//Insert second partition in the task pool.
				cltask_new_task(pool, new_task, current_offset);
				task[2] = 1;
			}
			else 
			//If it is a small partition, order it using insertion sort
			{
				for (int i=1; i<right-left+1; i++)
				{
					int j = i;
					while (j > 0 && array[left+j]<array[left+j-1])
					{
						SWAP(array[left+j-1], array[left+j]);
						j--;
					}
				}
			}
		}
		if (type == 1)
		{
			cltask_finish_task(pool, current_offset);
		}
	}
}


