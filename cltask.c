#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "common.h"
#include "cltask.h"


struct cltask *cltask_init(cl_context context, unsigned long size, unsigned long data_size)
{
	struct cltask *task;
	task = (struct cltask *) malloc(sizeof(struct cltask));
	if (task == NULL)
		return FAILURE;
	return SUCCESSS;
}


void cltask_free(cltask *task)
{
}


cl_mem cltask_get_pool(cltask *task)
{
}


void cltask_insert_task(cltask *task, void *task)
{
}


void cltask_print(cltask *task)
{
}


