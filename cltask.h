#ifndef CLTASK_H
#define CLTASK_H


#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "common.h"

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

struct cltask *cltask_init(cl_context context, cl_command_queue queue, cl_device_id device_id, unsigned long size, unsigned long data_size, void *task);
void cltask_free(struct cltask *task_pool);
cl_mem *cltask_get_pool(struct cltask *task_pool);
void cltask_print(struct cltask *task_pool);



#endif
