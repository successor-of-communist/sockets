#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include<sys/wait.h>
using namespace std;

int main()
{
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

    int maxfd=serFd,maxi=0;
    int clientFd[FD_SETSIZE];
    fd_set rset,allset;

    for(int i=0;i<FD_SETSIZE;i++){
        clientFd[i]=-1;
    }
    FD_ZERO(&allset);
    FD_SET(serFd,&allset);

    while (1)
    {
        rset = allset;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(serFd, &rset))
        {
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
            FD_SET(cliFd,&allset);
            for (int i = 0; i < FD_SETSIZE; i++)
            {
                if (clientFd[i] == -1)
                {
                    clientFd[i] = cliFd;
                    if (maxi < i)
                        maxi = i;
                    maxfd=max(maxfd,cliFd);
                    break;
                }
            }
            if(--nready<=0)
            continue;
        }
        for(int i=0;i<=maxi;i++){
            if(clientFd[i]==-1)
            continue;
            int clifd=clientFd[i];
            char buf[1024];
            int len = 0;
            if(FD_ISSET(clifd,&rset)){
                if((len=read(clifd,buf,sizeof(buf)))==0){
                    close(clifd);
                    FD_CLR(clifd,&allset);
                    clientFd[i]=-1;
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
