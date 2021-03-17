#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include"header.h"
#include"Socket.h"
class Task{
	int fd;
	int epollid;
	const int BUF_MAX=2048;
	void initTask();
	public:
	Task();
	Task(int _fd,int _epollid);
	void run();
};
class ThreadPool{
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	std::queue<Task*> que_task;
	std::vector<pthread_t*> que_thread;
	const int num_core=6;
	int runningtask;
	bool m_isDestory;
	static void * ThreadFunc(void* ptrData);
	public:
	ThreadPool();
	void addTask(Task *task);
	void creatThread();
	void destroy();
};

#endif
