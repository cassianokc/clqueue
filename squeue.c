#include "common.h"
#include "squeue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct squeue *squeue_init(unsigned long size, unsigned long data_size)
{
	struct squeue *queue;
	queue = malloc(sizeof(struct squeue));
	if (queue == NULL)
		return NULL;
	queue->size = size;
	queue->begin = 0;
	queue->end = 0;
	queue->count = 0;
	queue->data_size = data_size;
	queue->data = malloc(size*data_size);
	if (queue->data == NULL)
	{
		free(queue);
		return NULL;
	}
	return queue;
}

void squeue_free(struct squeue *queue)
{
	free(queue->data);
	free(queue);
}

bool squeue_is_empty(struct squeue *queue)
{
	return (queue->count == 0);
}

bool squeue_is_full(struct squeue *queue)
{
	return (queue->count == queue->size);
}

int squeue_remove(struct squeue *queue, void *data)
{
	unsigned char *aux=queue->data;
	if (squeue_is_empty(queue))
		return FAILURE;
	memcpy(data, aux+queue->begin*queue->data_size, queue->data_size);
	queue->begin = (queue->begin+1)%queue->size;
	queue->count--;
	return SUCCESS;
}

int squeue_insert(struct squeue *queue, void *data)
{
	unsigned char *aux=queue->data;
	if (squeue_is_full(queue))
		return FAILURE;
	memcpy(aux+queue->end*queue->data_size, data, queue->data_size);
	queue->end = (queue->end+1)%queue->size;
	queue->count++;
	return SUCCESS;
}

void squeue_print(struct squeue *queue)
{
	unsigned long i, j;
	unsigned char *aux=queue->data;
	printf("Struct squeue at %p\n", (void *) queue);
	printf("size=%lu	begin=%lu	end=%lu	data_size=%lu\n", queue->size,
		queue->begin, queue->end, queue->data_size);
	for (i=0; i<queue->count; i++)
	{
		printf("Block %lu: ", ((queue->begin+i)%queue->size));
		for (j=0; j<queue->data_size; j++)
		{
			printf("%u ", aux[((queue->begin+i)%queue->size)*queue->data_size+j]);
		}
		printf("\n");
	}
}

