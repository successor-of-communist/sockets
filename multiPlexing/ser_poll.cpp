#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include<sys/wait.h>
#include<poll.h>
using namespace std;

#define POLL_SIZE 2048
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

    int maxi;
    struct pollfd client[POLL_SIZE];
    client[0].fd=serFd;
    client[0].events=POLLRDNORM;
    for(int i=1;i<POLL_SIZE;i++)
    client[i].fd=-1;
    maxi=0;

    while (1)
    {
        int nready=poll(client,maxi+1,INFTIM);
        if(client[0].revents&POLLRDNORM){
            socklen_t cliaddrlen = sizeof(cliaddr);
            int cliFd = accept(serFd, (struct sockaddr *)&cliaddr, &cliaddrlen);
            if (cliFd < 0)
            {
                if (errno == EINTR)
                    continue;
                else
                {
                    perror("accept error");
                    break;
                }
            }
            for(int i=1;i<POLL_SIZE;i++){
                if(client[i].fd<0){
                    client[i].fd=cliFd;
                    client[i].events=POLLRDNORM;
                    maxi=max(i,maxi);
                    break;
                }
            }
            if(--nready<=0)
            continue;
        }
        for(int i=1;i<=maxi;i++){
            if(client[i].fd<0)
            continue;
            if(client[i].revents&(POLLRDNORM|POLLERR)){
            int clifd=client[i].fd;
            char buf[1024];
            int len = 0;
                if((len=read(clifd,buf,sizeof(buf)))<=0){
                    if(errno==ECONNRESET||len==0){
                    close(clifd);
                    client[i].fd=-1;
                    }
                    else{
                        perror("read error");
                        exit(1);
                    }
                }
                else{
                    buf[len]='\0';
                    cout<<buf;
                    write(clifd,buf,len);
                }
                if(--nready<=0)
                break;
            }

        }
    }
    close(serFd);

    return 0;
}
