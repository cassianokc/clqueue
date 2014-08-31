#ifndef CLTASK_H
#define CLTASK_H


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "common.h"


/*
*	This project implements a queue on the GPU memory, each GPU working group will dequeue jobs from 
* this queue and will process them.
*/


struct cltask
{
	unsigned long size;
	unsigned long data_size;
	cl_context context;
	cl_command_queue queue;
	cl_mem buffer;
	cl_program program;
	cl_kernel init;
};

struct cltask_gpu_pool
{
	int mutex;
	int state;
	//Missing data field because it is going to be calculated on the runtime
};

struct cltask_gpu_task
{
	unsigned long data_size;
	struct cltask_gpu_pool tasks;	
};

/* 
*	Allocates space for the cltask struct, compiles the cltask.cl and creates the kernel.
*	@param context Contains the OpenCL context in which the scheduler will be used.
* 	@param queue A command queue in the context.
*	@param device_id The device to create the scheduler.
*	@param size The maximum number of tasks that the scalonator can have(not yet implemented, the maximum
* number of tasks will always be 256).
*	@param data_size The number of bytes that each task can store(not yet implemented, the maximum number
* of bytes will always be 12).
*	@param task Pointer to the first task that will be added to the scheduler.
*	@return Returns a pointer to the cltask structure if successfull, returns NULL otherwise.
*/
struct cltask *cltask_init(cl_context context, cl_command_queue queue, cl_device_id device_id, unsigned long size, unsigned long data_size, void *task);

/*
*	Frees the cltask structure, releasing all resources used by it on the host and ond the device.
*	@param task_pool Pointer to the cltask structure to be released.
*/
void cltask_free(struct cltask *task_pool);

/*
*	Returns the pointer to the buffer containing the scheduler on the device memory.
*	@param task_pool pointer to the cltask structure that contains the buffer.
*	@return Returns a pointer to the cl_mem containing the scheduler on the device memory.
*/
cl_mem *cltask_get_pool(struct cltask *task_pool);



#endif
