#ifndef __SOCKET_H
#define __SOCKET_H
#include<sys/socket.h>
#include"Error.h"
#endif
#define SA struct sockaddr
    //report position of error and exit
    int Socket(int domain,int type,int protocol){
        int fd=socket(domain,type,protocol);
        if(fd<0){
            Error("socket");
        }
        return fd;
    }
    int Bind(int socket,const SA *addr,socklen_t addr_len){
        if(bind(socket,(SA*)&addr,addr_len)==-1){
            Error("bind");
        }
    }
    int Listen(int socket,int backlog){
        if(listen(socket,backlog)==-1){
            Error("listen");
        }
    }
    int Accept(int socket ,SA *addr,socklen_t *addr_len){
        return accept(socket,(SA*)addr,addr_len);
    }
    int Close(int socket){
        if(close(socket)==0)
        return 0;
        else
        Error("close");
    }