#ifndef __SOCKET_CPP
#define __SOCKET_CPP
#include"Socket.h"
#include"header.h"
#define SA struct sockaddr
void Error(const char * s){
        perror(s);
        exit(1);
    }
int Socket(int domain,int type,int protocol){
	int fd=socket(domain,type,protocol);
	if(fd<0){
		Error("socket");
	}
	return fd;
}
int Bind(int socket,const SA *addr,socklen_t addr_len){
	if(bind(socket,addr,addr_len)==-1){
		Error("bind");
	}
	return 0;
}
int Listen(int socket,int backlog){
	if(listen(socket,backlog)==-1){
		Error("listen");
	}
	return 0;
}
int Accept(int socket ,SA *addr,socklen_t *addr_len){
	return accept(socket,(SA*)addr,addr_len);
}
int Close(int socket){
	if(close(socket)==0)
		return 0;
	else
		Error("close");
	return -1;
}
int Read(int files,void *buf,size_t len){
	int l=read(files,buf,len);
	if(l<0)
		Error("read");
	return l;
}
int Write(int files,const void* buf,size_t len){
	int l=write(files,buf,len);
	if(l<0)
		Error("write");
	return l;
}
#endif
