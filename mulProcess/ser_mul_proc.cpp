#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include<sys/wait.h>
using namespace std;

typedef void Sigfunc(int);

void sig_chld(int signo){
    pid_t pid;
    int stat;
    pid=wait(&stat);
    printf("child %d terminated\n",pid);
    return;
}
Sigfunc *signal(int signo,Sigfunc * func){
    struct sigaction act,oact;
    act.sa_handler=func;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    if(signo==SIGALRM){
#ifdef SA_INTERRUPT
        act.sa_flags|=SA_INTERRUPT;
#endif
    }
    else{
#ifdef SA_RESTART
        act.sa_flags|=SA_RESTART;
#endif
    }
    if(sigaction(signo,&act,&oact)<0)
        return (SIG_ERR);
    return (oact.sa_handler);
}

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
    signal(SIGCHLD,sig_chld);

    while (1)
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
        char buf[1024];
        int len = 0;
        int chlfd;
        if ((chlfd = fork()) == 0)
        {
            close(serFd);
            while (1)
            {
                len = read(cliFd, buf, sizeof(buf));
                if (len < 0)
                {
                    perror("read error");
                    break;
                    exit(1);
                }
                else if (len == 0)
                    break;
                buf[len] = '\0';
                cout << buf;
            }
            exit(0);
        }
        close(cliFd);
    }
    close(serFd);

    return 0;
}
