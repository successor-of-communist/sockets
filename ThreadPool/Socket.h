#ifndef __SOCKET_H
#define __SOCKET_H
#include"header.h"
#define SA struct sockaddr
/*
 * wrap socket
 * */

//report position of error and exit
void Error(const char * s);

int Socket(int domain,int type,int protocol);

int Bind(int socket,const SA *addr,socklen_t addr_len);

int Listen(int socket,int backlog);

int Accept(int socket ,SA *addr,socklen_t *addr_len);

int Close(int socket);

int Read(int files,void *buf,size_t len);

int Write(int files,const void* buf,size_t len);

#endif
