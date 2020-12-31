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
#include<string>
#include<pthread.h>
#include<signal.h>
#define MAXLINE 4096
using namespace std;
class Client {
private:
    // 一个Client维护自己一个sock描述符就ok
    int sockfd;
    struct sockaddr_in clientaddr;
public:
    Client(){};
    // 客户端初始化，返回值为bool
    void Init();

    // 客户端启动，
    void Start();

    // 客户端关闭
    void Close();

    // 注册
    void Register(int);
    
    // 启动
    void Run(int);

    // 多线程：读线程、写线程，需要是全局函数！！
    // 用于读线程的函数
    static void* recvMessage(void*);
    // 用于写线程的函数
    static void* sendMessage(void*);
};
#endif