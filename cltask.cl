#ifndef CLTASK_CL
#define CLTASK_CL


#define TASK_PROCESSING -2
#define TASK_EMPTY -1
#define TASK_READY 0

#define DATA_SIZE 12
#define SIZE 128



int LOCK(int x);
int LOCK(int x)
{
	if (x==0)
		return 0;
	else
	{
		x=0;
		return 1;
	}
}
#define UNLOCK(X) (X = 1)




struct task
{
	/*
	 *	State = -1 Empty state, the task currently does not hold any data.
	 *	State = 0 Ready state, means that the task is ready to be processed.
	 *	State > 0 Dependant state, the task is waiting for -state tasks to be ready.
	 *	State = -2 Processing, the task is being processed.
	 */	
	int state;
	int lock;
	int parent_offset;
	unsigned char data[DATA_SIZE];	
};

struct pool
{
	int rand_x;
	int rand_y;
	int rand_z;
	int task_number;
	struct task tasks[SIZE];
};


/*
* 	Inits the scheduler data and inserts the first task.
*	@param task_pool The struct to be initialized.
*	@param first_task Pointer to the data of the first task that will be inserted in the scheduler.
*/
__kernel void cltask_gpu_init(global struct pool *task_pool, global unsigned char *first_task);

/*
*	Inserts a new task in the task pool.
*	@param task_pool The struct that the task will be added.
*	@param task Pointer to the data of the task that will be inserted in the scheduler.
*	@param parent_offset The offset of the parent which is dependant of this newly inserted task.
*	@return Returns 1 if the insertion was successfull, 0 otherwise.	
*/
int cltask_new_task(global struct pool *task_pool, void *task, int parent_offset);

/*
*	Removes a task from the task pool.
*	@param task_pool The struct from which the task will be removed.
*	@param task_offset The offset of the task in the task pool.
*/
void cltask_finish_task(global struct pool *task_pool, int task_offset);

/*
*	Retrieves a task from the task pool to be processed.
*	@param task_pool The scheduler struct.	
*	@param task_ret A pointer to a pointer in which will hold the task data.
*	@param offset_ret A pointer to a int which will hold the offset of the task retrieved.
*
*/
int cltask_retrieve_task(global struct pool *task_pool, global void **task_ret, int *offset_ret);

/*
* 	Makes a task given by its offset dependant of a number of tasks.
*	@param task_pool The task pool which contain the task.
*	@param offset The offset that identifies the task that will be dependant.
*	@param dependants The number of dependant tasks spawned by the task identified by offset.
*/
void cltask_set_dependant(global struct pool *task_pool, int offset, int dependants);

/*
* 	A pseudo randomic number generator
*	@param task_pool The task pool struct that contains the seeds for the random number.
*	@return A pseudo randomic integer
*/
int rand(global struct pool *task_pool);

/*
*	Seeds the pseudo randomic generator.
*	@param task_pool The struct which contains the seed.
*	@param seed The seed.
*/
void srand(global struct pool *task_pool, int seed);


__kernel void cltask_gpu_init(global struct pool *task_pool, global unsigned char *first_task)
{
	for (int i=1; i<SIZE; i++) 
	{
		task_pool->tasks[i].lock = 1;
		task_pool->tasks[i].state = TASK_EMPTY;
	}
	for (int i=0; i<DATA_SIZE; i++)
		task_pool->tasks[0].data[i] = first_task[i];
	task_pool->tasks[0].lock = 1;
	task_pool->tasks[0].state = TASK_READY; 
	task_pool->tasks[0].parent_offset = -1;
	task_pool->task_number=1;
	task_pool->rand_x = 123456789;
	task_pool->rand_y = 678912345;
	task_pool->rand_z = 987651234;
	srand(task_pool, (int) first_task);
}


int cltask_new_task(global struct pool *task_pool, void *task, int parent_offset)
{
	int offset;
	task_pool->task_number++;
	while (task_pool->task_number < SIZE)
	{
		offset=rand(task_pool)%SIZE;
		if (task_pool->tasks[offset].state==TASK_EMPTY && LOCK(task_pool->tasks[offset].lock)==1)
		{
			for (int i=0; i<DATA_SIZE; i++)
			{
				task_pool->tasks[offset].data[i] = ((unsigned char *) task)[i];
			}
			task_pool->tasks[offset].state = TASK_READY;
			task_pool->tasks[offset].parent_offset = parent_offset;
			UNLOCK(task_pool->tasks[offset].lock);
			return 1;
		}
	}
	return 0;
}

void cltask_finish_task(global struct pool *task_pool, int task_offset)
{
	task_pool->task_number--;
	LOCK(task_pool->tasks[task_offset].lock);
	task_pool->tasks[task_offset].state = TASK_EMPTY;
	if (task_offset != 0)
		task_pool->tasks[task_pool->tasks[task_offset].parent_offset].state--;
	UNLOCK(task_pool->tasks[task_offset].lock);
}

int cltask_retrieve_task(global struct pool *task_pool, global void **task_ret, int *offset_ret) 
{
	int offset;
	for (offset = rand(task_pool)%SIZE; task_pool->tasks[0].state != TASK_EMPTY; offset = rand(task_pool)%SIZE)
	{
		if (task_pool->tasks[offset].state==TASK_READY && LOCK(task_pool->tasks[offset].lock)==1)
		{
			*offset_ret = offset;
			task_pool->tasks[offset].state = TASK_PROCESSING;
			*task_ret = (global void *)task_pool->tasks[offset].data;
			return 1;
		}
	}
	return 0;
}

void cltask_set_dependant(global struct pool *task_pool, int offset, int dependants)
{
	task_pool->tasks[offset].state = dependants;	
	UNLOCK(task_pool->tasks[offset].lock);
	return;
}

void srand(global struct pool *task_pool, int seed)
{
	task_pool->rand_x ^= seed;
	task_pool->rand_y ^= seed<<3;
	task_pool->rand_z ^= seed<<7;	
}

int rand(global struct pool *task_pool)
{
	int t;
	task_pool->rand_x ^= task_pool->rand_x<<16;
	task_pool->rand_x ^= task_pool->rand_x>>5;
	task_pool->rand_x ^= task_pool->rand_x<<1;
	t = task_pool->rand_x;
	task_pool->rand_x = task_pool->rand_y;
	task_pool->rand_y = task_pool->rand_z;
	task_pool->rand_z = t^task_pool->rand_x^task_pool->rand_y;
	return task_pool->rand_z; 
}


#endif
