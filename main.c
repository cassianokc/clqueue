#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "common.h"
#include "cltask.h"

#define SIZE 2950 

int main(void)
{
	int task[] = {0, SIZE-1, 0};
	int values[SIZE];
	cl_context context;
	cl_device_id device_id;
	cl_platform_id platform_id;
	cl_command_queue queue;
	cl_program program;
	cl_kernel run;	
	cl_mem array;
	cl_int error;
	cl_uint ret_num_devices, ret_num_platforms;
	size_t global = 32;
	size_t local = 4;
	struct cltask *task_pool;
	FILE *fp;
	size_t source_size;
	char *source_str;
	srand(time(NULL));
	error = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);	
	assert(error == CL_SUCCESS);
	error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	assert(error == CL_SUCCESS);
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &error);
	assert(error == CL_SUCCESS);
	queue = clCreateCommandQueue(context, device_id, 0, &error);
	assert(error == CL_SUCCESS);
	task_pool = cltask_init(context, queue, device_id, 256, 12, &task);
	assert(task_pool != NULL);
	for (int i=0; i<SIZE; i++)
	{
		values[i] = rand();
	}
	array = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, SIZE*sizeof(int), values, &error); 
	assert(error==CL_SUCCESS);
	fp = fopen("main.cl", "r");
	assert(fp != NULL);
	source_str = (char *) malloc(100000);
	source_size = fread(source_str, 1, 100000, fp);
	fclose(fp);
	program = clCreateProgramWithSource(context, 1, (const char **) &source_str, (const size_t *) &source_size, &error);
	assert(error == CL_SUCCESS);
	free(source_str);
	error = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (error != CL_SUCCESS)
	{
		size_t length;
		char buffer[5000];
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
		printf("Error in compilation main.cl\n");
		printf("%s\n", buffer);
	}
	run = clCreateKernel(program, "run", &error);
	assert(error == CL_SUCCESS);
	error = clSetKernelArg(run, 0, sizeof(cl_mem), (cltask_get_pool(task_pool)));
	assert(error == CL_SUCCESS);	
	error = clSetKernelArg(run, 1, sizeof(cl_mem), &array);
	assert(error == CL_SUCCESS);	
	error = clEnqueueNDRangeKernel(queue, run, 1, NULL, &global, &local, 0, NULL, NULL);
	assert (error == CL_SUCCESS);
	error = clEnqueueReadBuffer(queue, array, 0, 0, SIZE*sizeof(int), values, 0, NULL, NULL);
	assert(error == CL_SUCCESS);
	error = clFlush(queue);
	assert(error == CL_SUCCESS);
	error = clFinish(queue);
	assert(error == CL_SUCCESS);
	cltask_free(task_pool);
	error = clReleaseMemObject(array);
	assert(error == CL_SUCCESS);
	error = clReleaseProgram(program);
	assert(error == CL_SUCCESS);
	error = clReleaseKernel(run);
	assert(error == CL_SUCCESS);
	error = clReleaseCommandQueue(queue);
	assert(error == CL_SUCCESS);
	error = clReleaseContext(context);
	return SUCCESS;
}

