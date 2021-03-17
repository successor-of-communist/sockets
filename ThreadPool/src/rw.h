#ifndef __RW_H
#define __RW_H
#include<unistd.h>
#include"Error.h"
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
