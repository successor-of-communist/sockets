#include<iostream>
#include<sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
int main(){
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(9876);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(clientfd,(struct sockaddr*)&addr,sizeof(addr))<0)
    char buf[100]="1";
    cout<<"w"<<endl;
    write(clientfd,buf,0);
    sleep(10);
    close(clientfd);
}
