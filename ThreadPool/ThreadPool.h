#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include<vector>
#include<queue>
#include"header.h"
#include"rw.h"
#include"Socket.h"
class Task{
    int fd;
    const int BUF_MAX=2048;
    void initTask(){
        fd=-1;
    }
    public:
    Task(){
        initTask();
    }
    void run(){
        char buf[BUF_MAX];
    int len;
    while(1){
        len=Read(*(int*)fd,buf,BUF_MAX);
        if(len==0){
            break;
        }
        buf[len]='\0';
        std::cout<<buf<<std::endl;
        Write(*(int*)fd,"get",3);
        std::string len_str=std::to_string(len);
        Write(*(int*)fd,len_str.c_str(),len_str.length());
    }
    Close(*(int*)fd);
    return;
    }

};
class ThreadPool{
    static pthread_mutex_t m_mutex;
    static pthread_cond_t m_cond;
    std::queue<Task*> que_task;
    std::vector<pthread_t*> que_thread;
    const int num_core=6;
    bool m_isDestory;
    static void * ThreadFunc(void* ptrData){
        ThreadPool *Thread=(ThreadPool*)ptrData;
        while(1){
        pthread_mutex_lock(&m_mutex);
        while(Thread->que_task.empty()&&!Thread->m_isDestory){
            pthread_cond_wait(&m_cond,&m_mutex);
        }
        Task *curtask;
        if(!Thread->que_task.empty()){
            curtask=Thread->que_task.front();
            Thread->que_task.pop();
        }
        pthread_mutex_unlock(&m_mutex);
        if(curtask!=nullptr){
            curtask->run();
        }
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
    }
    void addTask(Task *task){
        pthread_mutex_lock(&m_mutex);
        if(que_thread.size()>que_task.size()){
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