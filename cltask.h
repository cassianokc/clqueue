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
	//TODO: define number of kernels and proper names
	cl_kernel init;
	cl_kernel kernel2;
	
	
};

struct gpu_data
{
	unsigned long size;
	unsigned long 
}


struct cltask *cltask_init(cl_context context, unsigned long size, unsigned long data_size);
void cltask_free(cltask *task);
cl_mem cltask_get_pool(cltask *task);
void cltask_insert_task(cltask *task, void *task);
void cltask_print(cltask *task);



#endif
