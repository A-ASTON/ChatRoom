#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#define MAXLINE 4096
class Client {
private:
    // 一个Client维护自己一个sock描述符就ok
    int sockfd;
    struct sockaddr_in clientaddr;
public:
    Client();
    void test() {
        std::cout<<"Client Test~"<<std::endl;
    }
    // 客户端初始化，返回值为bool
    int Init();
    // 客户端启动，开始监听用户输入，然后发送到服务器
    int Start();
    // 客户端关闭
    int Close();
};
#endif