#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include<vector>
#include<queue>
#include"header.h"
#include"rw.h"
#include"Socket.h"
class Task{
	int fd;
	int epollid;
	const int BUF_MAX=2048;
	void initTask(){
		fd=-1;
	}
	public:
	Task(){
		initTask();
	}
	Task(int _fd,int _epollid){
		epollid=_epollid;
		fd=_fd;
	}
	void run(){
		char buf[BUF_MAX];
		int len;
		while(1){
			len=Read(fd,buf,BUF_MAX);
			if(len==0){
				break;
			}
			std::cout<<len<<std::endl;
			buf[len]='\0';
			std::cout<<buf<<std::endl;
			std::string str_ret="get";
			str_ret+=std::to_string(len);
			str_ret+="\n";
			Write(fd,str_ret.c_str(),str_ret.length());
			if(len<BUF_MAX){
				std::cout<<"wait"<<std::endl;
				struct epoll_event ev;	
				ev.data.fd=fd;
				ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
				epoll_ctl(epollid,EPOLL_CTL_MOD,fd,&ev);
				return;
			}
		}
				std::cout<<"quit"<<std::endl;
		Close(fd);
		return;
	}

};
class ThreadPool{
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	std::queue<Task*> que_task;
	std::vector<pthread_t*> que_thread;
	const int num_core=6;
	int runningtask;
	bool m_isDestory;
	static void * ThreadFunc(void* ptrData){
		ThreadPool *Thread=(ThreadPool*)ptrData;
		while(1){
			pthread_mutex_lock(&Thread->m_mutex);
			while(Thread->que_task.empty()&&!Thread->m_isDestory){
				pthread_cond_wait(&Thread->m_cond,&Thread->m_mutex);
			}
			Task *curtask;
			if(!Thread->que_task.empty()){
				curtask=Thread->que_task.front();
				Thread->que_task.pop();
				Thread->runningtask++;
			}
			pthread_mutex_unlock(&Thread->m_mutex);
			if(curtask!=nullptr){
				curtask->run();
			}
			pthread_mutex_lock(&Thread->m_mutex);
			Thread->runningtask--;
			pthread_mutex_unlock(&Thread->m_mutex);
			if(Thread->m_isDestory){
				pthread_exit(NULL);
				break;
			}
		}
		return NULL;
	}
	public:
	ThreadPool(){
		pthread_mutex_init(&m_mutex,NULL);
		pthread_cond_init(&m_cond,NULL);
		m_isDestory=0;
		runningtask=0;
	}
	void addTask(Task *task){
		pthread_mutex_lock(&m_mutex);
		if(que_thread.size()>que_task.size()+runningtask){
			que_task.push(task);
		}
		else if(que_thread.size()<num_core){
			creatThread();
			que_task.push(task);
		}
		pthread_mutex_unlock(&m_mutex);
		pthread_cond_broadcast(&m_cond);
	}
	void creatThread(){
		std::cout<<"jiale1ge"<<std::endl;
		pthread_t* threadid=(pthread_t*)malloc(sizeof(pthread_t));
		pthread_create(threadid,NULL,ThreadFunc,(void*)this);
		que_thread.push_back(threadid);
	}
	void destroy(){
		if(!m_isDestory){
			m_isDestory=1;
			pthread_cond_broadcast(&m_cond);
			for(auto iter:que_thread){
				pthread_join(*iter,NULL);
			}
			que_thread.clear();
			pthread_cond_destroy(&m_cond);
			pthread_mutex_destroy(&m_mutex);
		}
	}
};

#endif
