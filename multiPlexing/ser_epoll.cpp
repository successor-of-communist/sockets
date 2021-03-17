#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include<sys/wait.h>
#include<poll.h>
#include<sys/epoll.h>
using namespace std;

#define EPOLL_SIZE 2048
#define INFTIM -1

int main() {
    int serFd;
    struct sockaddr_in addr;
    struct sockaddr_in cliaddr;
    if ((serFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9560);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind error");
    }
    if (listen(serFd, 10) < 0)
    {
        perror("listen error");
    }

    int epollfd=epoll_create(10);
    struct epoll_event ev;
    struct epoll_event revs[EPOLL_SIZE];
    ev.data.fd=serFd;
    ev.events=EPOLLIN|EPOLLET;

    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,serFd,&ev)<0){
        perror("epoll ctl error");
        exit(2);
    }
    while(1){
        int nready;
        if((nready=epoll_wait(epollfd,revs,EPOLL_SIZE,INFTIM))<0){
            perror("epoll wait");
        }
        for(int i=0;i<nready;i++){
            if(revs[i].data.fd==serFd){
                socklen_t cliaddrlen = sizeof(cliaddr);
                int clifd = accept(serFd, (struct sockaddr *)&cliaddr, &cliaddrlen);
                if (clifd < 0)
                {
                    if (errno == EINTR)
                        continue;
                    else
                    {
                        perror("accept error");
                        break;
                    }
                }
                ev.data.fd=clifd;
                ev.events=EPOLLIN|EPOLLET;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clifd,&ev)<0){
                    perror("client epoll ctl");
                    exit(2);
                }
                continue;
            }

            //
            int clifd=revs[i].data.fd;
            char buf[1024];
            int len = 0;
            if((len=read(clifd,buf,sizeof(buf)))<=0){
                if(errno==ECONNRESET||len==0){
                    close(clifd);
                    ev=revs[i];
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,clifd,&ev);
                }
                else{
                    perror("read error");
                    exit(2);
                }
            }
            else{
                buf[len]='\0';
                cout<<buf;
                write(clifd,buf,len);
            }
        }
    }

    close(serFd);

    return 0;
}
