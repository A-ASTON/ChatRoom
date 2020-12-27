/*
 * @Author: your name
 * @Date: 2020-12-12 11:40:01
 * @LastEditTime: 2020-12-13 01:06:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /C++/chatRoom/Server.cpp
 */
#include "Server.h"
#include <iostream>

Server *Server::server = new Server;
Server &Server::GetServer() {
    // 返回一个引用，该引用就是server指针指向的server实例
    return *server;
}

// 服务器初始化
int Server::Init() {
    // 具体作用就是，初始化socket配置
    std::cout<<"Init~"<<std::endl;
    // 内存分配
    memset(&server->servaddr, 0, sizeof(servaddr));
    // ipv4
    server->servaddr.sin_family = AF_INET;
    // htonl函数，字节序转换host to network long,大端小端问题
    server->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 服务器监听8099端口
    server->servaddr.sin_port = htons(8099);
    return 1;
}

int Server::Start() {
    // 服务器启动，开始listen
    int sockfd, connfd;
    char buff[4096];
    int n;
    // 打开socket返回socket描述字，唯一标识一个socket，把它作为参数能进行一些读写操作
    // socket函数:参数1domain:协议域，参数2type:指定socket类型，参数3protocol:指定协议，0为根据type自动选择
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // 获取socket描述字,失败则提示错误信息
        std::cout<<"Create socket error!error:"<<strerror(errno)<<std::endl;
        return -1;
    }

    // bind函数,把一个地址族中的特定地址赋给socket，ip地址和端口号等等（根据socket的协议域）
    if(bind(sockfd, (struct sockaddr*)&(server->servaddr), sizeof(server->servaddr)) == -1) {
        std::cout<<"Bind socket error!error:"<<strerror(errno)<<std::endl;
        return -1;
    }

    // listen函数，开始监听，等待连接请求，第二个参数为相应socket可以排队的最大连接数
    if(listen(sockfd, 10) == -1) {
        std::cout<<"Listen socket error!error:"<<strerror(errno)<<std::endl;
        return -1;
    }

    // 等待连接
    std::cout<<"Waiting for connection......"<<std::endl;
    while(1) {
        // accept函数，接受请求并创建连接描述字
        if((connfd = accept(sockfd, (struct sockaddr*)NULL, NULL)) == -1) {
            std::cout<<"Accept socket error!error:"<<strerror(errno)<<std::endl;
            continue;
        }
        // 循环接收信息
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        std::cout<<"Recv message from client:"<<buff<<std::endl;
        // 接受完信息后关闭连接描述字
        close(connfd);
    }
    close(sockfd);
    return 0;
}

int Server::Close() {
    // 服务器关闭
    // close();
    return 0;
}

void Server::Serve() {
    // 服务器业务逻辑，如何处理与用户的聊天
    // 目前是，客户与服务器之间
}