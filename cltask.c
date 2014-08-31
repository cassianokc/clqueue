#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "common.h"
#include "cltask.h"


struct cltask *cltask_init(cl_context context, cl_command_queue queue, cl_device_id device_id, unsigned long size, unsigned long data_size, void *task)
{
	size_t global_size = 1;
	size_t local_size = 1;
	struct cltask *task_pool;
	FILE *fp;
	size_t source_size;
	char *source_str;

	cl_mem task_buffer;
	cl_int error;
	task_pool = (struct cltask *) malloc(sizeof(struct cltask));
	if (task_pool == NULL)
		return NULL;
	task_pool->size = size;
	task_pool->data_size = data_size;
	task_pool->context = context;
	task_pool->queue = queue;
	task_pool->buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, data_size*(size+sizeof(struct cltask_gpu_task))+sizeof(struct cltask_gpu_pool), NULL, &error);
	assert(error == CL_SUCCESS);
	fp = fopen("cltask.cl", "r");
	assert(fp != NULL);
	source_str = (char *) malloc(100000);
	assert(source_str != NULL);
	source_size = fread(source_str, 1, 100000, fp);
	fclose(fp);
	task_pool->program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *) &source_size, &error);
	assert(error == CL_SUCCESS);
	free(source_str);
	error = clBuildProgram(task_pool->program, 1, &device_id, NULL, NULL, NULL);
	if (error != CL_SUCCESS)
	{
		size_t length;
		char buffer[5000];
		error = clGetProgramBuildInfo(task_pool->program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
		assert(error == CL_SUCCESS);
		printf("Error in compilation cltask.cl\n");
		for (int i=0; i<length; i++)
			putchar(buffer[i]);
		exit(0);
	}
	task_pool->init = clCreateKernel(task_pool->program, "cltask_gpu_init", &error);
	assert(error == CL_SUCCESS);
	task_buffer = clCreateBuffer(task_pool->context, CL_MEM_USE_HOST_PTR, task_pool->data_size, task, &error);
	assert(error == CL_SUCCESS);
	error = clSetKernelArg(task_pool->init, 0, sizeof(cl_mem), &task_pool->buffer);
	assert(error == CL_SUCCESS);	
	error = clSetKernelArg(task_pool->init, 1, sizeof(cl_mem), &task_buffer);
	assert(error == CL_SUCCESS);	
	error = clEnqueueNDRangeKernel(task_pool->queue, task_pool->init, 1, NULL, &global_size, &local_size, 0, NULL, NULL);
	assert (error == CL_SUCCESS);
	error = clFlush(task_pool->queue);
	assert(error == CL_SUCCESS);
	error = clFinish(task_pool->queue);
	assert(error == CL_SUCCESS);
	error = clReleaseMemObject(task_buffer);
	assert(error == CL_SUCCESS);
	return task_pool;
}

void cltask_free(struct cltask *task_pool)
{
	cl_int error;
	error = clReleaseKernel(task_pool->init);
	assert(error == CL_SUCCESS);
	error = clReleaseProgram(task_pool->program);
	assert(error == CL_SUCCESS);
	error = clReleaseMemObject(task_pool->buffer);
	assert(error == CL_SUCCESS);
	free(task_pool);
}

cl_mem *cltask_get_pool(struct cltask *task_pool)
{
	return &task_pool->buffer;
}


