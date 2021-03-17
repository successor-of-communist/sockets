#ifndef __ERROR_H
#define __ERROR_H
#include<stdio.h>
#include<stdlib.h>
void Error(const char * s){
        perror(s);
        exit(1);
    }
#endif