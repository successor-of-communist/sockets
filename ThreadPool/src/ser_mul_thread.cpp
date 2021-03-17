#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/epoll.h>
#include<pthread.h>

#include "Socket.h"
#include "Error.h"
#include "ThreadPool.h"

#define EPOLL_MAX 2048

int main() {
    int listenfd,clientfd;
    struct sockaddr_in addr,clientaddr;
    int epollfd;
    struct epoll_event ev,tmpev,clientev,revs[EPOLL_MAX];
    socklen_t clientlen=sizeof(clientaddr);
    int nready;

    listenfd=Socket(AF_INET,SOCK_STREAM,0);
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(9999);
    Bind(listenfd,(struct sockaddr*)&addr,sizeof(addr));
    Listen(listenfd,10);

    epollfd=epoll_create(10);
    ev.data.fd=listenfd;
    ev.events=EPOLLIN|EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);

    ThreadPool pool;

    while(1){
        nready=epoll_wait(epollfd,revs,EPOLL_MAX,-1);
        for(int i=0;i<nready;i++){
            tmpev=revs[i];
            if(tmpev.data.fd==listenfd){
                clientfd=Accept(listenfd,(struct sockaddr*)&clientaddr,&clientlen);
                if(clientfd<0){
                    if(errno==EINTR){
                        continue;
                    }
                    Error("accpet");
                }
                clientev.data.fd=clientfd;
                clientev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&clientev);
                continue;
            }
            pthread_t threadid;
            Task *task=new Task(tmpev.data.fd,epollfd);
            pool.addTask(task);
        }
    }

    return 0;
}
