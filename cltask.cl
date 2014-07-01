struct task
{
	int mutex;
	int state;
	void *data;	
};


struct pool
{
	unsigned long data_size;
	struct task *tasks;

}
